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

require_once 'VMStorage.php';
require_once 'Voicemail.php';

/**
 * class VMStorageODBC
 * 
 */
class VMStorageODBC extends VMStorage {

	/** Private instance vars */
	private $server;
	private $login;
	private $pass;

	/**
	* VMStorageODBC -- Constructor
	* @param string server
	* @param string login
	* @param string pass
	*/
	public function __construct( $server,  $login,  $pass ) {
		
	}
	
	/**
	* VMStorageODBC -- Destructor
	*/
	public function __destruct() {
		
	}
	
	/**
	* load()
	*/
	public function load() {
		
	}

	/**
	* rename(a_voicemail_folder, a_voicemail_old_name, a_voicemail_new_name)
	* @param string a_voicemail_folder
	* @param string a_voicemail_old_name
	* @param string a_voicemail_new_name
	* @return bool
	*/
	public function rename( $foldName , $oldName , $newName ) {
		
	}
	
	/**
	* deleteFolder(a_voicemail_folder)
	* @param string a_voicemail_folder
	* @return bool
	*/
	public function deleteFolder( $folder ) {
	
	}

	/**
	* delete(a_voicemail_folderm, a_voicemail)
	* @param string a_voicemail_folder
	* @param string a_voicemail_name
	* @return bool
	*/
	public function delete( $folder , $vm ) {
		
	}

	/**
	* connect()
	* @return bool
	*/
	public function connect() {
		
	}

	/**
	* disconnect()
	* @return bool
	*/
	public function disconnect() {
		
	}

} // end of VMStorageODBC
?>
