/*
 *  Copyright (C) 2004, 2005, 2006, 2009, 2008, 2009, 2010 Savoir-Faire Linux Inc.
 *  Author: Pierre-Luc Beaudoin <pierre-luc.beaudoin@savoirfairelinux.com>
 *  Author: Alexandre Bourget <alexandre.bourget@savoirfairelinux.com>
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

#ifndef CONFIGURATIONMANAGER_H
#define CONFIGURATIONMANAGER_H

#include "configurationmanager-glue.h"
#include <dbus-c++/dbus.h>

/**
 * This structure contains:
 *		hash code (unique identifier),
 * 		clock rate,
 * 		payload type,
 * 		mime type,
 * 		mime subtype,
 * 		bitrate,
 * 		bandwidth,
 * 		additional information (description)
 */
typedef ::DBus::Struct<std::string, uint32_t, uint8_t, std::string,
std::string, double, double, std::string> DbusAudioCodec;

typedef ::DBus::Struct<std::string, uint32_t, uint8_t, std::string,
std::string, double, double, std::string> DbusVideoCodec;

/**
 * "always offer video (boolean)"
 * "preferred device (string)"
 * "preferred resolution (struct : uint uint)"
 * "preferred framerate (struct : uint uint)"
 */
typedef ::DBus::Struct< ::DBus::Struct<uint32_t, uint32_t>,
::DBus::Struct<uint32_t, uint32_t>, std::string, bool> DbusVideoSettings;

/**
 * Note that this exception is not the same as the one
 * in the "sfl" namespace. This exception is thrown
 * over dbus.
 */
namespace DBus
{

/**
 * This exception is thrown when some operation should be
 * performed on some account that cannot be found.
 */
class NonexistentAccountException : public DBus::Error
{
    public:
        /**
         * @param accoundID The accountID that was attempted to obtain.
         */
        NonexistentAccountException (const std::string& accountID) :
                DBus::Error ("NonexistentAccountException", (std::string ("Account id \"") + accountID + std::string ("\" cannot be found.")).c_str()) {
        }
};

}

class ConfigurationManager: public org::sflphone::SFLphone::ConfigurationManager_adaptor,
        public DBus::IntrospectableAdaptor,
        public DBus::ObjectAdaptor
{
    public:

        ConfigurationManager (DBus::Connection& connection);
        static const char* SERVER_PATH;

    private:
        std::vector<std::string> shortcutsKeys;

        /**
         * Utility structure helping to convert from Dbus struct to something more human-readable.
         * TODO Do the same for codecs.
         */
        struct VideoSettings {
            VideoSettings() {
                resolution.width = 0;
                resolution.height = 0;

                framerate.numerator = 0;
                framerate.denominator = 1;

                device = "";
                alwaysOfferVideo = false;
            }

            /**
             * Construct a VideoSettings object from a Dbus structure.
             * @param settings The Dbus representation of this object.
             */
            VideoSettings (const DbusVideoSettings& settings) {
                resolution.width = settings._1._1;
                resolution.height = settings._1._2;

                framerate.numerator = settings._2._1;
                framerate.denominator = settings._2._2;

                device = settings._3;
                alwaysOfferVideo = settings._4;
            }

            /**
             * @return The Dbus structure representing this object.
             */
            DbusVideoSettings toDbusType() {
                DbusVideoSettings settings;
                settings._1._1 = resolution.width;
                settings._1._2 = resolution.height;
                settings._2._1 = framerate.numerator;
                settings._2._2 = framerate.denominator;
                settings._3  = device;
                settings._4 = alwaysOfferVideo;
                return settings;
            }

            bool alwaysOfferVideo;

            std::string device;

            struct {
                uint32_t width;
                uint32_t height;
            } resolution;

            struct {
                uint32_t numerator;
                uint32_t denominator;
            } framerate;
        };

    public:
        /**
         * @param accountID The account ID
         * @return  All parameters of the specified account.
         */
        std::map<std::string, std::string> getAccountDetails (
            const std::string& accountID);

        /**
         * Send new account parameters, or account parameters changes, to the core.
         * The hash table is not required to be complete, only the updated parameters may be specified.
         * Account settings are written to the configuration file when sflphone properly quits.
         * After calling this method, the core will emit the signal accountsChanged with the updated data.
         * The client must subscribe to this signal and use it to update its internal data structure.
         *
         * @param accountID The account ID
         * @param details A map with key:value pairs containing new settings for the given account.
         * @return
         */
        void setAccountDetails (const std::string& accountID, const std::map<
                                std::string, std::string>& details);

        /**
         * Add a new account. When created, the signal accountsChanged is emitted.
         * The clients must then call getAccountList to update their internal data structure.
         * If no details are specified, the default parameters are used.
         * The core tries to register the account as soon it is created.
         *
         * @param details A map with key:value pairs containing new settings for the new account.
         */
        std::string addAccount (const std::map<std::string, std::string>& details);

        std::vector<std::string> getSupportedTlsMethod (void);

        /**
         * @return a list of all the MIME subtypes for the supported audio codec.
         */
        std::vector<std::string> getAllAudioCodecMimeSubtypes();

        /**
         * @return a structure containing the following fields in order :
         * 		hash code,
         * 		clock rate,
         * 		payload type,
         * 		mime type,
         * 		mime subtype,
         * 		bitrate,
         * 		bandwidth,
         * 		description.
         */
        DbusAudioCodec getAudioCodecDetails (const std::string& codecMimeType);

        /**
         * @return a vector of all the audio codecs that are available. Same as calling getAudioCodecMimeSubtypes() in a loop, then
         * getAudioCodecDetails.
         */
        std::vector<DbusAudioCodec> getAllAudioCodecs();

        /**
         * @return a vector of all the audio codecs that are available. Same as calling getAudioCodecMimeSubtypes() in a loop, then
         * getAudioCodecDetails.
         */
        std::vector<DbusAudioCodec> getAllVideoCodecs();

        /**
         * @param accountID A string representing the account for which to return the list of active codec.
         * @return A Dbus Array containing all of the video codecs for the given account.
         */
        std::vector<DbusAudioCodec> getAllActiveAudioCodecs (
            const std::string& accountID);

        /**
         * @param accountID A string representing the account for which to return the list of active codec.
         * @return A Dbus Array containing all of the video codecs for the given account.
         */
        std::vector<DbusAudioCodec> getAllActiveVideoCodecs (
            const std::string& accountID);

        /**
         * @param codecIdentifiers A vector containing all of the audio codec identifiers, in some specified order.
         * @param accountID The account identifier for which to set the new active codec list.
         * TODO Change the order of arguments.
         */
        void setActiveAudioCodecs (const std::vector<std::string>& codecIdentifiers,
                                   const std::string& accountID);

        /**
         * @param codecIdentifiers A vector containing all of the video codec identifiers, in some specified order.
         * @param accountID The account identifier for which to set the new active codec list.
         * TODO Change the order of arguments.
         */
        void setActiveVideoCodecs (const std::vector<std::string>& codecIdentifiers,
                                   const std::string& accountID);

        /**
         * @param settings A structure containing members of the form :
         * 			"always offer video (boolean)"
         * 			"preferred device (string)"
         * 			"preferred resolution (struct : uint uint)"
         * 			"preferred framerate (struct : uint uint)"
         * @param accountID The account identifier for which to set the video settings.
         * @throw NonexistentAccountException if the given account id cannot be found.
         */
        void setVideoSettings (const std::string& accountID,
                               const DbusVideoSettings& settings) throw (DBus::NonexistentAccountException);

        /**
         * @param accountID The account identifier for which to set the video settings.
         */
        DbusVideoSettings getVideoSettings (const std::string& accountID) throw (DBus::NonexistentAccountException);

        void removeAccount (const std::string& accoundID);
        void deleteAllCredential (const std::string& accountID);
        std::vector<std::string> getAccountList();
        void sendRegister (const std::string& accoundID, const int32_t& expire);

        std::map<std::string, std::string> getTlsSettingsDefault (void);
        void setIp2IpDetails (const std::map<std::string, std::string>& details);
        std::map<std::string, std::string> getIp2IpDetails (void);
        std::map<std::string, std::string> getCredential (
            const std::string& accountID, const int32_t& index);
        int32_t getNumberOfCredential (const std::string& accountID);
        void setCredential (const std::string& accountID, const int32_t& index,
                            const std::map<std::string, std::string>& details);
        void setNumberOfCredential (const std::string& accountID,
                                    const int32_t& number);

        std::vector<std::string> getAudioPluginList();
        void setInputAudioPlugin (const std::string& audioPlugin);
        void setOutputAudioPlugin (const std::string& audioPlugin);
        std::vector<std::string> getAudioOutputDeviceList();
        void setAudioOutputDevice (const int32_t& index);
        void setAudioInputDevice (const int32_t& index);
        void setAudioRingtoneDevice (const int32_t& index);
        std::vector<std::string> getAudioInputDeviceList();
        std::vector<std::string> getCurrentAudioDevicesIndex();
        int32_t getAudioDeviceIndex (const std::string& name);
        std::string getCurrentAudioOutputPlugin (void);
        std::string getEchoCancelState (void);
        void setEchoCancelState (const std::string& state);
        std::string getNoiseSuppressState (void);
        void setNoiseSuppressState (const std::string& state);

        std::vector<std::string> getToneLocaleList();
        std::vector<std::string> getPlaybackDeviceList();
        std::vector<std::string> getRecordDeviceList();
        std::string getVersion();
        std::vector<std::string> getRingtoneList();
        int32_t getAudioManager (void);
        void setAudioManager (const int32_t& api);

        bool isMd5CredentialHashing (void);
        void setMd5CredentialHashing (const bool& enabled);
        int32_t isIax2Enabled (void);
        int32_t isRingtoneEnabled (void);
        void ringtoneEnabled (void);
        std::string getRingtoneChoice (void);
        void setRingtoneChoice (const std::string& tone);

        void ringtoneEnabled (const std::string& accountID);

        /**
         * @return true if ringtone is enabled, false otherwise
         */
        int32_t isRingtoneEnabled (const std::string& accountID);

        std::string getRingtoneChoice (const std::string& accountID);

        void setRingtoneChoice (const std::string& accountID,
                                const std::string& tone);

        std::string getRecordPath (void);
        void setRecordPath (const std::string& recPath);

        void setHistoryLimit (const int32_t& days);
        int32_t getHistoryLimit (void);

        int32_t getMailNotify (void);
        void setMailNotify (void);

        std::map<std::string, int32_t> getAddressbookSettings (void);
        void setAddressbookSettings (const std::map<std::string, int32_t>& settings);
        std::vector<std::string> getAddressbookList (void);
        void setAddressbookList (const std::vector<std::string>& list);

        void setAccountsOrder (const std::string& order);

        std::map<std::string, std::string> getHookSettings (void);
        void setHookSettings (const std::map<std::string, std::string>& settings);

        std::map<std::string, std::string> getHistory (void);
        void setHistory (const std::map<std::string, std::string>& entries);

        std::map<std::string, std::string> getTlsSettings (
            const std::string& accountID);
        void setTlsSettings (const std::string& accountID, const std::map<
                             std::string, std::string>& details);

        std::string getAddrFromInterfaceName (const std::string& interface);

        std::vector<std::string> getAllIpInterface (void);
        std::vector<std::string> getAllIpInterfaceByName (void);

        std::map< std::string, std::string > getShortcuts();
        void setShortcuts (const std::map<std::string, std::string>& shortcutsMap);
};

#endif//CONFIGURATIONMANAGER_H
