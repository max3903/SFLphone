/*
 *  Copyright (C) 2004, 2005, 2006, 2009, 2008, 2009, 2010 Savoir-Faire Linux Inc.
 *  Author: Pierre-Luc Beaudoin <pierre-luc.beaudoin@savoirfairelinux.com>
 *  Author: Emmanuel Milou <emmanuel.milou@savoirfairelinux.com>
 *  Author: Guillaume Carmel-Archambault <guillaume.carmel-archambault@savoirfairelinux.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  Additional permission under GNU GPL version 3 section 7:
 *
 *  If you modify this program, or any covered work, by linking or
 *  combining it with the OpenSSL project's OpenSSL library (or a
 *  modified version of that library), containing parts covered by the
 *  terms of the OpenSSL or SSLeay licenses, Savoir-Faire Linux Inc.
 *  grants you additional permission to convey the resulting work.
 *  Corresponding Source for a non-source form of such a combination
 *  shall include the source code for the parts of OpenSSL used as well
 *  as that of the covered work.
 */

#include <global.h>
#include <configurationmanager.h>
#include <sstream>
#include "../manager.h"
#include "sip/sipvoiplink.h"
#include "sip/sipaccount.h"

#include "CodecFactory.h"

const char* ConfigurationManager::SERVER_PATH =
		"/org/sflphone/SFLphone/ConfigurationManager";

ConfigurationManager::ConfigurationManager(DBus::Connection& connection) :
	DBus::ObjectAdaptor(connection, SERVER_PATH) {
	shortcutsKeys.push_back("pick_up");
	shortcutsKeys.push_back("hang_up");
	shortcutsKeys.push_back("popup_window");
	shortcutsKeys.push_back("toggle_pick_up_hang_up");
	shortcutsKeys.push_back("toggle_hold");
}

std::map<std::string, std::string> ConfigurationManager::getAccountDetails(
		const std::string& accountID) {

	_debug("ConfigurationManager: get account details %s", accountID.c_str());
	return Manager::instance().getAccountDetails(accountID);
}

std::map<std::string, std::string> ConfigurationManager::getTlsSettingsDefault(
		void) {

	std::map<std::string, std::string> tlsSettingsDefault;

	tlsSettingsDefault.insert(std::pair<std::string, std::string>(
			TLS_LISTENER_PORT, DEFAULT_SIP_TLS_PORT));
	tlsSettingsDefault.insert(std::pair<std::string, std::string>(
			TLS_CA_LIST_FILE, ""));
	tlsSettingsDefault.insert(std::pair<std::string, std::string>(
			TLS_CERTIFICATE_FILE, ""));
	tlsSettingsDefault.insert(std::pair<std::string, std::string>(
			TLS_PRIVATE_KEY_FILE, ""));
	tlsSettingsDefault.insert(std::pair<std::string, std::string>(TLS_PASSWORD,
			""));
	tlsSettingsDefault.insert(std::pair<std::string, std::string>(TLS_METHOD,
			"TLSv1"));
	tlsSettingsDefault.insert(std::pair<std::string, std::string>(TLS_CIPHERS,
			""));
	tlsSettingsDefault.insert(std::pair<std::string, std::string>(
			TLS_SERVER_NAME, ""));
	tlsSettingsDefault.insert(std::pair<std::string, std::string>(
			TLS_VERIFY_SERVER, "true"));
	tlsSettingsDefault.insert(std::pair<std::string, std::string>(
			TLS_VERIFY_CLIENT, "true"));
	tlsSettingsDefault.insert(std::pair<std::string, std::string>(
			TLS_REQUIRE_CLIENT_CERTIFICATE, "true"));
	tlsSettingsDefault.insert(std::pair<std::string, std::string>(
			TLS_NEGOTIATION_TIMEOUT_SEC, "2"));
	tlsSettingsDefault.insert(std::pair<std::string, std::string>(
			TLS_NEGOTIATION_TIMEOUT_MSEC, "0"));

	return tlsSettingsDefault;
}

std::map<std::string, std::string> ConfigurationManager::getIp2IpDetails(void) {

	std::map<std::string, std::string> ip2ipAccountDetails;

	SIPAccount *sipaccount = (SIPAccount *) Manager::instance().getAccount(
			IP2IP_PROFILE);

	if (!sipaccount) {
		_error("ConfigurationManager: could not find account");
		return ip2ipAccountDetails;
	}

	ip2ipAccountDetails.insert(std::pair<std::string, std::string>(ACCOUNT_ID,
			IP2IP_PROFILE));
	ip2ipAccountDetails.insert(std::pair<std::string, std::string>(
			SRTP_KEY_EXCHANGE, sipaccount->getSrtpKeyExchange()));
	ip2ipAccountDetails.insert(std::pair<std::string, std::string>(SRTP_ENABLE,
			sipaccount->getSrtpEnable() ? "true" : "false"));
	ip2ipAccountDetails.insert(
			std::pair<std::string, std::string>(SRTP_RTP_FALLBACK,
					sipaccount->getSrtpFallback() ? "true" : "false"));
	ip2ipAccountDetails.insert(std::pair<std::string, std::string>(
			ZRTP_DISPLAY_SAS, sipaccount->getZrtpDisplaySas() ? "true"
					: "false"));
	ip2ipAccountDetails.insert(std::pair<std::string, std::string>(
			ZRTP_HELLO_HASH, sipaccount->getZrtpHelloHash() ? "true" : "false"));
	ip2ipAccountDetails.insert(std::pair<std::string, std::string>(
			ZRTP_NOT_SUPP_WARNING, sipaccount->getZrtpNotSuppWarning() ? "true"
					: "false"));
	ip2ipAccountDetails.insert(std::pair<std::string, std::string>(
			ZRTP_DISPLAY_SAS_ONCE, sipaccount->getZrtpDiaplaySasOnce() ? "true"
					: "false"));
	ip2ipAccountDetails.insert(std::pair<std::string, std::string>(
			LOCAL_INTERFACE, sipaccount->getLocalInterface()));
	std::stringstream portstr;
	portstr << sipaccount->getLocalPort();
	ip2ipAccountDetails.insert(std::pair<std::string, std::string>(LOCAL_PORT,
			portstr.str()));

	std::map<std::string, std::string> tlsSettings;
	tlsSettings = getTlsSettings(IP2IP_PROFILE);
	std::copy(tlsSettings.begin(), tlsSettings.end(), std::inserter(
			ip2ipAccountDetails, ip2ipAccountDetails.end()));

	return ip2ipAccountDetails;

}

void ConfigurationManager::setIp2IpDetails(const std::map<std::string,
		std::string>& details) {
	std::map<std::string, std::string> map_cpy = details;
	std::map<std::string, std::string>::iterator it;

	SIPAccount *sipaccount = (SIPAccount *) Manager::instance().getAccount(
			IP2IP_PROFILE);

	if (!sipaccount) {
		_error("ConfigurationManager: could not find account");
	}

	it = map_cpy.find(LOCAL_INTERFACE);
	if (it != details.end())
		sipaccount->setLocalInterface(it->second);

	it = map_cpy.find(LOCAL_PORT);
	if (it != details.end())
		sipaccount->setLocalPort(atoi(it->second.data()));

	it = map_cpy.find(SRTP_ENABLE);
	if (it != details.end())
		sipaccount->setSrtpEnable((it->second == "true"));

	it = map_cpy.find(SRTP_RTP_FALLBACK);
	if (it != details.end())
		sipaccount->setSrtpFallback((it->second == "true"));

	it = map_cpy.find(SRTP_KEY_EXCHANGE);
	if (it != details.end())
		sipaccount->setSrtpKeyExchange(it->second);

	it = map_cpy.find(ZRTP_DISPLAY_SAS);
	if (it != details.end())
		sipaccount->setZrtpDisplaySas((it->second == "true"));

	it = map_cpy.find(ZRTP_NOT_SUPP_WARNING);
	if (it != details.end())
		sipaccount->setZrtpNotSuppWarning((it->second == "true"));

	it = map_cpy.find(ZRTP_HELLO_HASH);
	if (it != details.end())
		sipaccount->setZrtpHelloHash((it->second == "true"));

	it = map_cpy.find(ZRTP_DISPLAY_SAS_ONCE);
	if (it != details.end())
		sipaccount->setZrtpDiaplaySasOnce((it->second == "true"));

	setTlsSettings(IP2IP_PROFILE, details);

	Manager::instance().saveConfig();

	// Update account details to the client side
	accountsChanged();

	// Reload account settings from config
	Manager::instance().getAccount(IP2IP_PROFILE)->loadConfig();

}

std::map<std::string, std::string> ConfigurationManager::getTlsSettings(
		const std::string& section) {

	std::map<std::string, std::string> tlsSettings;

	SIPAccount *sipaccount = (SIPAccount *) Manager::instance().getAccount(
			IP2IP_PROFILE);

	if (!sipaccount)
		return tlsSettings;

	std::stringstream portstr;
	portstr << sipaccount->getTlsListenerPort();
	tlsSettings.insert(std::pair<std::string, std::string>(TLS_LISTENER_PORT,
			portstr.str()));
	tlsSettings.insert(std::pair<std::string, std::string>(TLS_ENABLE,
			sipaccount->getTlsEnable()));
	tlsSettings.insert(std::pair<std::string, std::string>(TLS_CA_LIST_FILE,
			sipaccount->getTlsCaListFile()));
	tlsSettings.insert(std::pair<std::string, std::string>(
			TLS_CERTIFICATE_FILE, sipaccount->getTlsCertificateFile()));
	tlsSettings.insert(std::pair<std::string, std::string>(
			TLS_PRIVATE_KEY_FILE, sipaccount->getTlsPrivateKeyFile()));
	tlsSettings.insert(std::pair<std::string, std::string>(TLS_PASSWORD,
			sipaccount->getTlsPassword()));
	tlsSettings.insert(std::pair<std::string, std::string>(TLS_METHOD,
			sipaccount->getTlsMethod()));
	tlsSettings.insert(std::pair<std::string, std::string>(TLS_CIPHERS,
			sipaccount->getTlsCiphers()));
	tlsSettings.insert(std::pair<std::string, std::string>(TLS_SERVER_NAME,
			sipaccount->getTlsServerName()));
	tlsSettings.insert(std::pair<std::string, std::string>(TLS_VERIFY_SERVER,
			sipaccount->getTlsVerifyServer() ? "true" : "false"));
	tlsSettings.insert(std::pair<std::string, std::string>(TLS_VERIFY_CLIENT,
			sipaccount->getTlsVerifyClient() ? "true" : "false"));
	tlsSettings.insert(std::pair<std::string, std::string>(
			TLS_REQUIRE_CLIENT_CERTIFICATE,
			sipaccount->getTlsRequireClientCertificate() ? "true" : "false"));
	tlsSettings.insert(std::pair<std::string, std::string>(
			TLS_NEGOTIATION_TIMEOUT_SEC,
			sipaccount->getTlsNegotiationTimeoutSec()));
	tlsSettings.insert(std::pair<std::string, std::string>(
			TLS_NEGOTIATION_TIMEOUT_MSEC,
			sipaccount->getTlsNegotiationTimeoutMsec()));

	return tlsSettings;
}

void ConfigurationManager::setTlsSettings(const std::string& section,
		const std::map<std::string, std::string>& details) {

	std::map<std::string, std::string> map_cpy = details;
	std::map<std::string, std::string>::iterator it;

	SIPAccount * sipaccount = (SIPAccount *) Manager::instance().getAccount(
			IP2IP_PROFILE);

	if (!sipaccount) {
		_debug("ConfigurationManager: Error: No valid account in set TLS settings");
		return;
	}

	it = map_cpy.find(TLS_LISTENER_PORT);
	if (it != details.end())
		sipaccount->setTlsListenerPort(atoi(it->second.data()));

	it = map_cpy.find(TLS_ENABLE);
	if (it != details.end())
		sipaccount->setTlsEnable(it->second);

	it = map_cpy.find(TLS_CA_LIST_FILE);
	if (it != map_cpy.end())
		sipaccount->setTlsCaListFile(it->second);

	it = map_cpy.find(TLS_CERTIFICATE_FILE);
	if (it != map_cpy.end())
		sipaccount->setTlsCertificateFile(it->second);

	it = map_cpy.find(TLS_PRIVATE_KEY_FILE);
	if (it != map_cpy.end())
		sipaccount->setTlsPrivateKeyFile(it->second);

	it = map_cpy.find(TLS_PASSWORD);
	if (it != map_cpy.end())
		sipaccount->setTlsPassword(it->second);

	it = map_cpy.find(TLS_METHOD);
	if (it != map_cpy.end())
		sipaccount->setTlsMethod(it->second);

	it = map_cpy.find(TLS_CIPHERS);
	if (it != map_cpy.end())
		sipaccount->setTlsCiphers(it->second);

	it = map_cpy.find(TLS_SERVER_NAME);
	if (it != map_cpy.end())
		sipaccount->setTlsServerName(it->second);

	it = map_cpy.find(TLS_VERIFY_CLIENT);
	if (it != map_cpy.end())
		sipaccount->setTlsVerifyClient((it->second == "true") ? true : false);

	it = map_cpy.find(TLS_REQUIRE_CLIENT_CERTIFICATE);
	if (it != map_cpy.end())
		sipaccount->setTlsRequireClientCertificate(
				(it->second == "true") ? true : false);

	it = map_cpy.find(TLS_NEGOTIATION_TIMEOUT_SEC);
	if (it != map_cpy.end())
		sipaccount->setTlsNegotiationTimeoutSec(it->second);

	it = map_cpy.find(TLS_NEGOTIATION_TIMEOUT_MSEC);
	if (it != map_cpy.end())
		sipaccount->setTlsNegotiationTimeoutMsec(it->second);

	Manager::instance().saveConfig();

	// Update account details to the client side
	accountsChanged();

}

std::map<std::string, std::string> ConfigurationManager::getCredential(
		const std::string& accountID, const int32_t& index) {

	Account *account = Manager::instance().getAccount(accountID);

	std::map<std::string, std::string> credentialInformation;

	if (account->getType() != "SIP")
		return credentialInformation;

	SIPAccount *sipaccount = (SIPAccount *) account;

	if (index == 0) {
		std::string username = sipaccount->getUsername();
		std::string password = sipaccount->getPassword();
		std::string realm = sipaccount->getRealm();

		credentialInformation.insert(std::pair<std::string, std::string>(
				USERNAME, username));
		credentialInformation.insert(std::pair<std::string, std::string>(
				PASSWORD, password));
		credentialInformation.insert(std::pair<std::string, std::string>(REALM,
				realm));
	} else {

		// TODO: implement for extra credentials
		std::string username = sipaccount->getUsername();
		std::string password = sipaccount->getPassword();
		std::string realm = sipaccount->getRealm();

		credentialInformation.insert(std::pair<std::string, std::string>(
				USERNAME, username));
		credentialInformation.insert(std::pair<std::string, std::string>(
				PASSWORD, password));
		credentialInformation.insert(std::pair<std::string, std::string>(REALM,
				realm));
	}

	return credentialInformation;
}

int32_t ConfigurationManager::getNumberOfCredential(
		const std::string& accountID) {

	SIPAccount *sipaccount = (SIPAccount *) Manager::instance().getAccount(
			accountID);
	return sipaccount->getCredentialCount();
}

void ConfigurationManager::setNumberOfCredential(const std::string& accountID,
		const int32_t& number) {
	/*
	 if (accountID != ACCOUNT_NULL || !accountID.empty()) {
	 SIPAccount *sipaccount = (SIPAccount *)Manager::instance().getAccount(accountID);
	 sipaccount->setCredentialCount(number);
	 }
	 */
}

void ConfigurationManager::setCredential(const std::string& accountID,
		const int32_t& index, const std::map<std::string, std::string>& details) {
	Manager::instance().setCredential(accountID, index, details);
}

void ConfigurationManager::deleteAllCredential(const std::string& accountID) {
	Manager::instance().deleteAllCredential(accountID);
}

void ConfigurationManager::setAccountDetails(const std::string& accountID,
		const std::map<std::string, std::string>& details) {
	Manager::instance().setAccountDetails(accountID, details);
}

void ConfigurationManager::sendRegister(const std::string& accountID,
		const int32_t& expire) {
	Manager::instance().sendRegister(accountID, expire);
}

std::string ConfigurationManager::addAccount(const std::map<std::string,
		std::string>& details) {
	return Manager::instance().addAccount(details);
}

void ConfigurationManager::removeAccount(const std::string& accoundID) {
	return Manager::instance().removeAccount(accoundID);
}

std::vector<std::string> ConfigurationManager::getAccountList() {
	return Manager::instance().getAccountList();
}

//TODO
std::vector<std::string> ConfigurationManager::getToneLocaleList() {
	std::vector<std::string> ret;
	return ret;
}

//TODO
std::string ConfigurationManager::getVersion() {
	std::string ret("");
	return ret;
}

//TODO
std::vector<std::string> ConfigurationManager::getRingtoneList() {
	std::vector<std::string> ret;
	return ret;
}

std::vector<std::string> ConfigurationManager::getSupportedTlsMethod(void) {
	std::vector<std::string> method;
	method.push_back("Default");
	method.push_back("TLSv1");
	method.push_back("SSLv2");
	method.push_back("SSLv3");
	method.push_back("SSLv23");
	return method;
}

std::vector<std::string> ConfigurationManager::getAllAudioCodecMimeSubtypes(
		void) {
	CodecFactory& factory = CodecFactory::getInstance();
	return factory.getAllMimeSubtypes();
}

DbusAudioCodec ConfigurationManager::getAudioCodecDetails(
		const std::string& codecSubMimeType) {
	CodecFactory& factory = CodecFactory::getInstance();
	const sfl::Codec* codec = factory.getCodec(codecSubMimeType); // TODO Make sure that the codec does exists

	DbusAudioCodec codecDescription;
	codecDescription._1 = codec->hashCode();
	codecDescription._2 = codec->getClockRate();
	codecDescription._3 = codec->getPayloadType();
	codecDescription._4 = codec->getMimeType();
	codecDescription._5 = codec->getMimeSubtype();
	codecDescription._6 = codec->getBitRate();
	codecDescription._7 = codec->getBandwidth();
	codecDescription._8 = codec->getDescription();

	return codecDescription;
}

std::vector<DbusAudioCodec> ConfigurationManager::getAllAudioCodecs() {
	std::vector<DbusAudioCodec> output;

	CodecFactory& factory = CodecFactory::getInstance();

	std::vector<const AudioCodec*> codecs = factory.getAllAudioCodecs();
	std::vector<const AudioCodec*>::iterator it;
	for (it = codecs.begin(); it != codecs.end(); it++) {
		const AudioCodec* codec = (*it);

		DbusAudioCodec codecDescription;
		codecDescription._1 = codec->hashCode();
		codecDescription._2 = codec->getClockRate();
		codecDescription._3 = codec->getPayloadType();
		codecDescription._4 = codec->getMimeType();
		codecDescription._5 = codec->getMimeSubtype();
		codecDescription._6 = codec->getBitRate();
		codecDescription._7 = codec->getBandwidth();
		codecDescription._8 = codec->getDescription();

		output.push_back(codecDescription);
	}

	return output;
}

std::vector<DbusAudioCodec> ConfigurationManager::getAllVideoCodecs() {
	std::vector<DbusAudioCodec> output;

	CodecFactory& factory = CodecFactory::getInstance();

	std::vector<const sfl::VideoCodec*> codecs = factory.getAllVideoCodecs();
	std::vector<const sfl::VideoCodec*>::iterator it;
	for (it = codecs.begin(); it != codecs.end(); it++) {
		const sfl::VideoCodec* codec = (*it);

		DbusAudioCodec codecDescription;
		codecDescription._1 = codec->hashCode();
		codecDescription._2 = codec->getClockRate();
		codecDescription._3 = codec->getPayloadType();
		codecDescription._4 = codec->getMimeType();
		codecDescription._5 = codec->getMimeSubtype();
		codecDescription._6 = codec->getBitRate();
		codecDescription._7 = codec->getBandwidth();
		codecDescription._8 = codec->getDescription();

		output.push_back(codecDescription);
	}

	return output;
}

std::vector<DbusAudioCodec> ConfigurationManager::getAllActiveAudioCodecs(
		const std::string& accountID) {
	_info("Sending active codec list for account \"%s\" ...", accountID.c_str ());

	CodecFactory& factory = CodecFactory::getInstance();
	CodecOrder audioCodecOrder = factory.getDefaultAudioCodecOrder();

	Account* account = Manager::instance().getAccount(accountID);
	if (account != NULL) {
		audioCodecOrder = account->getActiveAudioCodecs();
	} else {
		_error("Could not return active codec list for non-existing account id \"%s\". Sending audio defaults.", accountID.c_str());
	}

	_info("Account \"%s\" has %d active audio codecs.", accountID.c_str(), audioCodecOrder.size());

	std::vector<DbusAudioCodec> output;
	CodecOrder::iterator it;
	for (it = audioCodecOrder.begin(); it != audioCodecOrder.end(); it++) {
		const AudioCodec* codec =
				static_cast<const AudioCodec*> (factory.getCodec((*it)));
		if (codec != NULL) { // TODO Catch exception instead

			DbusAudioCodec codecDescription;
			codecDescription._1 = codec->hashCode();
			codecDescription._2 = codec->getClockRate();
			codecDescription._3 = codec->getPayloadType();
			codecDescription._4 = codec->getMimeType();
			codecDescription._5 = codec->getMimeSubtype();
			codecDescription._6 = codec->getBitRate();
			codecDescription._7 = codec->getBandwidth();
			codecDescription._8 = codec->getDescription();

			output.push_back(codecDescription);
			_debug("Sending \"%s\" (id \"%s\")", codec->getMimeSubtype().c_str(), codec->hashCode().c_str());
		}
	}

	return output;
}

std::vector<DbusAudioCodec> ConfigurationManager::getAllActiveVideoCodecs(
		const std::string& accountID)
{
	_info("Sending active video codec list for account \"%s\" ...", accountID.c_str ());

	CodecFactory& factory = CodecFactory::getInstance();
	CodecOrder videoCodecOrder = factory.getDefaultVideoCodecOrder();

	Account* account = Manager::instance().getAccount(accountID);
	if (account != NULL) {
		videoCodecOrder = account->getActiveVideoCodecs();
	} else {
		_error("Could not return active codec list for non-existing account id \"%s\". Sending video defaults.", accountID.c_str());
	}

	_info("Account \"%s\" has %d active video codecs.", accountID.c_str(), videoCodecOrder.size());

	std::vector<DbusAudioCodec> output;
	CodecOrder::iterator it;
	for (it = videoCodecOrder.begin(); it != videoCodecOrder.end(); it++) {
		const sfl::VideoCodec* codec =
				dynamic_cast<const sfl::VideoCodec*> (factory.getCodec((*it)));
		if (codec != NULL) { // TODO Catch exception instead

			DbusAudioCodec codecDescription;
			codecDescription._1 = codec->hashCode();
			codecDescription._2 = codec->getClockRate();
			codecDescription._3 = codec->getPayloadType();
			codecDescription._4 = codec->getMimeType();
			codecDescription._5 = codec->getMimeSubtype();
			codecDescription._6 = codec->getBitRate();
			codecDescription._7 = codec->getBandwidth();
			codecDescription._8 = codec->getDescription();

			output.push_back(codecDescription);
			_debug("Sending \"%s\" (id \"%s\")", codec->getMimeSubtype().c_str(), codec->hashCode().c_str());
		}
	}

	return output;
}

void ConfigurationManager::setActiveAudioCodecs(
		const std::vector<std::string>& codecIdentifiers,
		const std::string& accountID) {
	_info ("Setting audio codecs for account id \"%s\"", accountID.c_str());

	// Set the new codec order.
	Account* account = Manager::instance().getAccount(accountID);
	if (!account) {
		_error("Account id \"%s\" cannot be found.", accountID.c_str());
		return;
	}

	// Create a CodecOrder object from the hash codes contained in the structures.
	CodecOrder ordering;

	std::vector<std::string>::const_iterator it;
	for (it = codecIdentifiers.begin(); it != codecIdentifiers.end(); it++) {
		_debug("Setting codec ID \"%s\" for account \"%s\"", (*it).c_str(), accountID.c_str());
		ordering.push_back((*it));
	}

	account->setActiveAudioCodecs(ordering);
}

void ConfigurationManager::setActiveVideoCodecs(
		const std::vector<std::string>& codecIdentifiers,
		const std::string& accountID) {
	_info ("Setting video codecs for account id \"%s\"", accountID.c_str());

	// Create a CodecOrder object from the hash codes contained in the structures.
	CodecOrder ordering;

	std::vector<std::string>::const_iterator it;
	for (it = codecIdentifiers.begin(); it != codecIdentifiers.end(); it++) {
		_debug("Setting video codec ID \"%s\" for account \"%s\"", (*it).c_str(), accountID.c_str());
		ordering.push_back((*it));
	}

	// Set the new codec order.
	Account* account = Manager::instance().getAccount(accountID);
	if (!account) {
		_error("Account id \"%s\" cannot be found.", accountID.c_str());
		return;
	}
	account->setActiveVideoCodecs(ordering);
}



void ConfigurationManager::setVideoSettings(const std::string& accountID, const DbusVideoSettings& settings) throw(DBus::NonexistentAccountException)
{
	Account* account = Manager::instance().getAccount(accountID);
	if (!account) {
		throw DBus::NonexistentAccountException(accountID);
	}

	VideoSettings videoSettings(settings);

	// Always offer video
	_debug("Settings video parameters for account %s : always offer video : %d", accountID.c_str(), videoSettings.alwaysOfferVideo);
	account->setAlwaysOfferVideo(videoSettings.alwaysOfferVideo);

	// Video device name
	_debug("Settings video parameters for account %s : preferred device name : %s", accountID.c_str(), videoSettings.device.c_str());
	account->setPreferredVideoDevice(videoSettings.device);

	// Video format
	sfl::VideoFormat format;
	_debug("Settings video parameters for account %s : preferred framerate : %d/%d", accountID.c_str(), videoSettings.framerate.numerator, videoSettings.framerate.denominator);
	format.setFramerate(videoSettings.framerate.numerator, videoSettings.framerate.denominator);

	_debug("Settings video parameters for account %s : preferred resolution : %d x %d", accountID.c_str(), videoSettings.resolution.width, videoSettings.resolution.height);
	format.setWidth(videoSettings.resolution.width);
	format.setHeight(videoSettings.resolution.height);

	account->setPreferredVideoFormat(format);
}

DbusVideoSettings ConfigurationManager::getVideoSettings(const std::string& accountID) throw(DBus::NonexistentAccountException)
{
	Account* account = Manager::instance().getAccount(accountID);
	if (!account) {
		throw DBus::NonexistentAccountException(accountID);
	}

	// Build a VideoSettings structure.
	VideoSettings videoSettings;
	sfl::VideoFormat format = account->getPreferredVideoFormat();
	videoSettings.resolution.width = format.getWidth();
	videoSettings.resolution.height = format.getHeight();
	videoSettings.framerate.numerator = format.getPreferredFrameRate().getNumerator();
	videoSettings.framerate.denominator = format.getPreferredFrameRate().getDenominator();
	videoSettings.device = account->getPreferredVideoDevice();
	videoSettings.alwaysOfferVideo = account->isAlwaysOfferVideo();

	return videoSettings.toDbusType();
}

std::vector<std::string> ConfigurationManager::getAudioPluginList() {

	std::vector<std::string> v;

	v.push_back(PCM_DEFAULT);
	// v.push_back(PCM_DMIX);
	v.push_back(PCM_DMIX_DSNOOP);

	return v;
}

void ConfigurationManager::setInputAudioPlugin(const std::string& audioPlugin) {
	return Manager::instance().setInputAudioPlugin(audioPlugin);
}

void ConfigurationManager::setOutputAudioPlugin(const std::string& audioPlugin) {
	return Manager::instance().setOutputAudioPlugin(audioPlugin);
}

std::vector<std::string> ConfigurationManager::getAudioOutputDeviceList() {
	return Manager::instance().getAudioOutputDeviceList();
}

std::vector<std::string> ConfigurationManager::getAudioInputDeviceList() {
	return Manager::instance().getAudioInputDeviceList();
}

void ConfigurationManager::setAudioOutputDevice(const int32_t& index) {
	return Manager::instance().setAudioDevice(index, SFL_PCM_PLAYBACK);
}

void ConfigurationManager::setAudioInputDevice(const int32_t& index) {
	return Manager::instance().setAudioDevice(index, SFL_PCM_CAPTURE);
}

void ConfigurationManager::setAudioRingtoneDevice(const int32_t& index) {
	return Manager::instance().setAudioDevice(index, SFL_PCM_RINGTONE);
}

std::vector<std::string> ConfigurationManager::getCurrentAudioDevicesIndex() {
	return Manager::instance().getCurrentAudioDevicesIndex();
}

int32_t ConfigurationManager::getAudioDeviceIndex(const std::string& name) {
	return Manager::instance().getAudioDeviceIndex(name);
}

std::string ConfigurationManager::getCurrentAudioOutputPlugin(void) {
	return Manager::instance().getCurrentAudioOutputPlugin();
}

std::string ConfigurationManager::getEchoCancelState(void) {
	return Manager::instance().getEchoCancelState();
}

void ConfigurationManager::setEchoCancelState(const std::string& state) {
	Manager::instance().setEchoCancelState(state);
}

std::string ConfigurationManager::getNoiseSuppressState(void) {
	return Manager::instance().getNoiseSuppressState();
}

void ConfigurationManager::setNoiseSuppressState(const std::string& state) {
	Manager::instance().setNoiseSuppressState(state);
}

std::vector<std::string> ConfigurationManager::getPlaybackDeviceList() {
	std::vector<std::string> ret;
	return ret;
}

std::vector<std::string> ConfigurationManager::getRecordDeviceList() {
	std::vector<std::string> ret;
	return ret;

}

bool ConfigurationManager::isMd5CredentialHashing(void) {
	return Manager::instance().preferences.getMd5Hash();
}

void ConfigurationManager::setMd5CredentialHashing(const bool& enabled) {
	Manager::instance().preferences.setMd5Hash(enabled);
}

int32_t ConfigurationManager::isIax2Enabled(void) {
	return Manager::instance().isIax2Enabled();
}

void ConfigurationManager::ringtoneEnabled(const std::string& accountID) {
	Manager::instance().ringtoneEnabled(accountID);
}

int32_t ConfigurationManager::isRingtoneEnabled(const std::string& accountID) {
	return Manager::instance().isRingtoneEnabled(accountID);
}

std::string ConfigurationManager::getRingtoneChoice(
		const std::string& accountID) {
	return Manager::instance().getRingtoneChoice(accountID);
}

void ConfigurationManager::setRingtoneChoice(const std::string& accountID,
		const std::string& tone) {
	Manager::instance().setRingtoneChoice(accountID, tone);
}

std::string ConfigurationManager::getRecordPath(void) {
	return Manager::instance().getRecordPath();
}

void ConfigurationManager::setRecordPath(const std::string& recPath) {
	Manager::instance().setRecordPath(recPath);
}

int32_t ConfigurationManager::getHistoryLimit(void) {
	return Manager::instance().getHistoryLimit();
}

void ConfigurationManager::setHistoryLimit(const int32_t& days) {
	Manager::instance().setHistoryLimit(days);
}

void ConfigurationManager::setAudioManager(const int32_t& api) {
	Manager::instance().setAudioManager(api);
}

int32_t ConfigurationManager::getAudioManager(void) {
	return Manager::instance().getAudioManager();
}

void ConfigurationManager::setMailNotify(void) {
	Manager::instance().setMailNotify();
}

int32_t ConfigurationManager::getMailNotify(void) {
	return Manager::instance().getMailNotify();
}

std::map<std::string, int32_t> ConfigurationManager::getAddressbookSettings(
		void) {
	return Manager::instance().getAddressbookSettings();
}

void ConfigurationManager::setAddressbookSettings(const std::map<std::string,
		int32_t>& settings) {
	Manager::instance().setAddressbookSettings(settings);
}

std::vector<std::string> ConfigurationManager::getAddressbookList(void) {
	return Manager::instance().getAddressbookList();
}

void ConfigurationManager::setAddressbookList(
		const std::vector<std::string>& list) {
	Manager::instance().setAddressbookList(list);
}

std::map<std::string, std::string> ConfigurationManager::getHookSettings(void) {
	return Manager::instance().getHookSettings();
}

void ConfigurationManager::setHookSettings(const std::map<std::string,
		std::string>& settings) {
	Manager::instance().setHookSettings(settings);
}

void ConfigurationManager::setAccountsOrder(const std::string& order) {
	Manager::instance().setAccountsOrder(order);
}

std::map<std::string, std::string> ConfigurationManager::getHistory(void) {
	return Manager::instance().send_history_to_client();
}

void ConfigurationManager::setHistory(
		const std::map<std::string, std::string>& entries) {
	Manager::instance().receive_history_from_client(entries);
}

std::string ConfigurationManager::getAddrFromInterfaceName(
		const std::string& interface) {

	std::string address = SipVoipLink::instance("")->getInterfaceAddrFromName(
			interface);

	return address;
}

std::vector<std::string> ConfigurationManager::getAllIpInterface(void) {

	std::vector<std::string> vector;
	SipVoipLink * sipLink = NULL;
	sipLink = SipVoipLink::instance("");

	if (sipLink != NULL) {
		vector = sipLink->getAllIpInterface();
	}

	return vector;
}

std::vector<std::string> ConfigurationManager::getAllIpInterfaceByName(void) {
	std::vector<std::string> vector;
	SipVoipLink * sipLink = NULL;
	sipLink = SipVoipLink::instance("");

	if (sipLink != NULL) {
		vector = sipLink->getAllIpInterfaceByName();
	}

	return vector;
}

std::map<std::string, int32_t> ConfigurationManager::getShortcuts() {

	std::map<std::string, int> shortcutsMap;
	int shortcut;

	for (int i = 0; i < (int) shortcutsKeys.size(); i++) {
		std::string key = shortcutsKeys.at(i);
		shortcut = Manager::instance().getConfigInt("Shortcuts", key);
		shortcutsMap.insert(std::pair<std::string, int>(key, shortcut));
	}

	return shortcutsMap;
}

void ConfigurationManager::setShortcuts(
		const std::map<std::string, int32_t>& shortcutsMap) {

	std::map<std::string, int> map_cpy = shortcutsMap;
	std::map<std::string, int>::iterator it;

	for (int i = 0; i < (int) shortcutsKeys.size(); i++) {
		std::string key = shortcutsKeys.at(i);
		it = map_cpy.find(key);
		if (it != shortcutsMap.end()) {
			Manager::instance().setConfig("Shortcuts", key, it->second);
		}
	}

	Manager::instance().saveConfig();
}

