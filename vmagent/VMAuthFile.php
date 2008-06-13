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

require_once 'VMAuth.php';

/**
 * class VMAuthFile
 * 
 */
class VMAuthFile extends VMAuth {

	/** Private instance vars */
	private $file;
	
	/**
	* VMAuthFile -- Constructor
	* @param string login
	* @param string password
	* @param string context ("default" by default)
	* @param string file_configuration ("/etc/asterisk/voicemail.conf" by default)
	*/
	public function __construct( $login="", $pass="", $context="default", $file="/etc/asterisk/voicemail.conf" ) {
		$this->login = $login;
		$this->pass = $pass;
		$this->context = $context;
		$this->file = $file;
	}

	/**
	* VMAuthFile -- Destructor
	*/
	public function __destruct() {
		
	}

	/**
	* getContext()
	* @return associated context
	*/
	public function getContext() {
		return $this->context;
	}

	/**
	* setContext(a_context)
	* @param string a_context
	*/
	public function setContext( $context ) {
		$this->context = $context;
	}

	/**
	* getLogin()
	* @return string login
	*/
	public function getLogin() {
		return $this->login;
	}

	/**
	* setLogin(a_login)
	* @param string a_login
	*/
	public function setLogin( $login ) {
		$this->login = $login;
	}
	
	/**
	* getPass()
	* @return string password
	*/
	public function getPass() {
		return $this->pass;
	}
	
	/**
	* setPass(a_password)
	* @param string a_password
	*/
	public function setPass( $pass ) {
		$this->pass = $pass;
	}

	/**
	* login()
	* @return TRUE if well registered in $VOICEMAIL_CONF file (default is /etc/asterisk/voicemail.conf), FALSE otherwise
	*/
	public function login() {
		$fd = fopen( $this->file , "r" );
		if( !$fd ) {
			echo "  <error>";
			echo "Could not open the VOICEMAIL_CONF file : $VOICEMAIL_CONF\n"; 
			echo "</error>\n";
			return FALSE;
		}
		
		$valid = FALSE;
		$cont = "";
		while( ! feof( $fd ) ) {
			$buf = fgets( $fd, 4096 );
			$line = trim( $buf );
			/** Saving current context */ 
			$pattern = '/^\[(.*)\]$/';
			if( preg_match( $pattern, $line, $matches ) ) {
				$cont = $matches[1];
			}
			/** Matching current mailbox with own datas */
			$pattern  = '/^\s*(\w+)\s*=>\s*(\d+),.*$/';
			if( preg_match( $pattern, $line, $matches ) ) {
				if( $cont == $this->getContext() && $matches[1] == $this->login && $matches[2] == $this->pass ) {
					$valid = TRUE;
					return $valid;
				}
			}
		}
		fclose( $fd );
		return $valid;
	}

	/**
	* logoff()
	* @return always TRUE due to login method
	*/
	public function logoff() {
		return TRUE;
	}

} // end of VMAuthFile
?>
