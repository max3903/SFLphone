<?php
/*
 *  Copyright (C) 2006-2007 Savoir-Faire Linux inc.
 *  Author: Florian DESPORTES <florian.desportes@savoirfairelinux.com>
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
 */
 
require_once 'Voicemail.php';
require_once 'VoicemailFolder.php';
require_once 'VMApiMan.php';
require_once 'VMAuthFile.php';
require_once 'VMAuthDB.php';
require_once 'VMStorageFile.php';
require_once 'VMStorageODBC.php';
require_once 'VMStorageIMAP.php';

/**
 * class VMAgent
 * 
 */
class VMAgent {

	/**
	 * Instance vars
	 */
	private $vmAPI;     // ApiMan
	private $vmAuth;    // Auth (abstract)
	private $vmStorage; // VMSotrage (abstract)
	private $vmList;	// Voicemail's Folders list
	
	/**
	 * VMAgent -- Constuctor
	 * @param string type_authentication
	 * @param string type_file_storage
	 */
	public function __construct($login, $pass, $context) {
		$vmAPI = new VMApiMan(); // ApiMan
		$this->autoAuthentication($login, $pass, $context);
		$this->checkStorage($login, $pass, $context);
	}
	
	/**
	 * VMAgent -- Destrucor
	 */
	public function __destruct() {
		
	}
	
	/**
	 * autoAuthentication(login, password, context)
	 * @param string login
	 * @param string password
	 * @param string context
	 */
	private function autoAuthentication($login, $pass, $context) {
		$fd = fopen("/etc/asterisk/voicemail.conf", "r");
		if( !$fd ) {
			echo "<error>";
			echo "Could not open the VOICEMAIL_CONF file : $VOICEMAIL_CONF\n"; 
			echo "</error>\n";
			return FALSE;
		}
		$inDBpattern = '/^\s*searchcontexts=yes/';
		$authDB = FALSE;
		while( ! feof($fd) ) {
			$buf = fgets($fd, 4096);
			$line = trim($buf);
			if( preg_match($inDBpattern, $line) ) {
				$authDB = TRUE;
			}
		}
		fclose($fd);
		if( $authDB ) {
			$this->vmAuth = new VMAuthDB();// $login, $pass, $context );
		} else {
			$this->vmAuth = new VMAuthFile($login, $pass, $context);
		}
	}
	
	/**
	 * checkStorage(login, password, context)
	 * @param string login
	 * @param string password
	 * @param string context
	 */
	private function checkStorage($login, $pass, $context) {
		$fd = fopen("/etc/asterisk/voicemail.conf", "r");
		if( !$fd ) {
			echo "<error>";
			echo "Could not open the VOICEMAIL_CONF file : $VOICEMAIL_CONF\n"; 
			echo "</error>\n";
			return FALSE;
		}
		
		$inODBCpattern = array( "/^\s*odbcstorage=(.*)/" => "",
								"/^\s*odbctable=(.*)/" => "" );
		$inIMAPpattern = array( "/^\s*imapserver=(.*)/" => "",
								"/^\s*imapport=(.*)/" => "",
								"/^\s*authuser=(.*)/" => "",
								"/^\s*authpassword=(.*)/" => "",
								"/^\s*imapfolder=(.*)/" => "" );
		$authODBC = FALSE;
		$authIMAP = FALSE;
		while( ! feof($fd) ) {
			$buf = fgets($fd, 4096);
			$line = trim($buf);
			foreach( $inODBCpattern as $key => $val ) {
				if( preg_match($key, $line, $matches) ) {
					$inODBCpattern[$key] = $matches[1];
				}
			}
			foreach( $inIMAPpattern as $key => $val ) {
				if( preg_match($key, $line, $matches) ) {
					$inODBCpattern[$key] = $matches[1];
				}
			}
		}
		fclose($fd);
		$inODBC = TRUE;
		$inIMAP = TRUE;
		foreach( $inODBCpattern as $key => $val ) {
				if( empty($val) ) {
				$inODBC = FALSE;
			}
		}
		foreach( $inIMAPpattern as $key => $val ) {
			if( empty($val) ) {
				$inIMAP = FALSE;
			}
		}
		if( $inODBC ) {
			$this->vmAuth = new VMStorageODBC();// $login, $pass, $context );
		} else if( $inIMAP ) {
			$this->vmAuth = new VMStorageIMAP($login, $pass, $context);
		} else {
			$this->vmStorage = new VMStorageFile("/var/spool/asterisk/voicemail/". $context ."/". $login);
		}
	}
	
	/**
	 * getVMList()
	 * @return Array<Voicemail>
	 */
	public function getVMList() {
		return $this->vmStorage->getLstFolders();
	}
	
	/**
	 * getVMFByName(a_voicemail_folder_name)
	 * @param string a_voicemail_folder_name
	 * @return VoicemailFolder the_voicemail_folder
	 */
	public function getVMFByName($name) {
		foreach( $this->vmStorage->getLstFolders() as $vmf ) {
			if( strcmp($vmf->getName(), $name) == 0 ) {
				return $vmf;
			}
		}
	}
	
	/**
	 * getVMByName(a_voicemail_folder_name, a _voicemail_name)
	 * @param string a_voicemail_folder_name
	 * @param string a_voicemail_name
	 * @return Voicemail the_voicemail
	 */
	public function getVMByName($fol, $name) {
		return $this->getVMFByName($fol)->getVMByName($name);
	}
	
	/**
	 * getVMAt(num)
	 * @param int num 
	 * @return Voicemail
	 * @access public
	 */
	public function getVMAt($num) {
		if( $num >= 0 && $num < count($this->vmList) ) {
			return $this->vmList[$num];
		} else {
			return -1;
		}
	}

	/**
	 * login()
	 * @param string login 
	 * @param string pass 
	 * @param string context
	 * @return TRUE if well registered, FALSE otherwise
	 */
	public function login($login, $pass, $context="default") {
		$this->vmAuth->setLogin($login);
		$this->vmAuth->setPass($pass);
		$this->vmAuth->setContext($context);
		if( $this->vmAuth->login() ) {
			return TRUE;
		} else {
			return FALSE;
		}
	}
	
	/**
	 * logout()
	 * @return always TRUE
	 */	
	public function logout() {
		return $this->vmAuth->logoff();
	}
	
	/**
	 * load()
	 */
	public function load() {
		$this->vmStorage->load();
	}
	
	/**
	 * getSound(a_voicemail_folder, a_voicemail_to_get_the_sound)
	 * @param string a_voicemail_folder
	 * @param string a_voicemail_to_get_the_sound
	 * @return bool TRUE if the choosen voicemail exists and we have a sound file, FALSE otherwise
	 */
	public function getSound($folder, $voicemail) {
		return $this->vmStorage->getSound($folder, $voicemail);
	}
	
	/**
	 * rename(a_voicemail_folder, a_voicemail_current_name, a_voicemail_future_name)
	 * @param string a_voicemail_folder
	 * @param string a_voicemail_current_name
	 * @param string a_voicemail_future_name
	 * @return bool TRUE if all files (txt+sound) have been well renamed, FALSE otherwise
	 */
	public function rename($folderName, $oldVMName, $newVMName) {
		return $this->vmStorage->rename($folderName, $oldVMName, $newVMName);
	}
	
	/**
	 * delete(a_voicemail_folder, a_voicemail_name)
	 * @param string a_voicemail_folder
	 * @param string a_voicemail_name
	 * @return bool TRUE if all files (txt+sound) have been well deletes, FALSE otherwise
	 */
	public function deleteFolder($folder) {
		return $this->vmStorage->deleteFolder($folder);
	}
	
	/**
	 * delete(a_voicemail_folder, a_voicemail_name)
	 * @param string a_voicemail_folder
	 * @param string a_voicemail_name
	 * @return bool TRUE if all files (txt+sound) have been well deletes, FALSE otherwise
	 */
	public function delete($folder, $vmName) {
		return $this->vmStorage->delete($folder, $vmName);
	}

} // end of VMAgent
?>
