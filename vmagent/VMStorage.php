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

require_once 'VMAgent.php';
require_once 'Voicemail.php';

/**
 * class VMStorage
 * 
 */
abstract class VMStorage {

	/** Protected instance vars: */
	protected $lstFolders = array();

	/**
	* VMStorageDB -- Constructor
	*/
	public function __construct() {
		
	}

	/**
	* VMStorageDB -- Destructor
	*/
	public function __destruct() {
		
	}

	/**
	* renameAll()
	* @return bool
	*/
	public function renameAll($folder) {
		foreach( $lstFolders[$folder] as $vm ) {
			$this->rename($vm);
		}
	}

	/**
	* load()
	*/
	abstract public function load();
	
	/**
	* rename(a_voicemail_folder, a_voicemail_old_name, a_voicemail_new_name)
	* @param string a_voicemail_folder
	* @param string a_voicemail_old_name
	* @param string a_voicemail_new_name
	* @return bool
	*/
	abstract public function rename($foldName, $oldName, $newName);

	/**
	* deleteFolder(a_voicemail_folder)
	* @param string a_voicemail_folder
	* @return bool
	*/
	abstract public function deleteFolder($folder);	
	
	/**
	* delete(a_voicemail_folder, a_voicemail)
	* @param string a_voicemail_folder
	* @param string a_voicemail_name
	* @return bool
	*/
	abstract public function delete($folder, $vm);
	
	/**
	 * getLstFolders()
	 * @return Array<VoicemailFolde> lstFolders
	 */
	public function getLstFolders() {
		return $this->lstFolders;
	}

} // end of VMStorage
?>
