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
 * class VMStorageFile
 * 
 */
class VMStorageFile extends VMStorage {

	/** Private instance vars */
	private $pathFolder;

	/**
	* VMStorageFile -- Constructor
	* @param Folder folder 
	*/
	public function __construct($path="/var/spool/asterisk/voicemail") {
		$this->pathFolder = $path;
	}
	
	/**
	* VMStorageFile -- Destructor
	*/
	public function __destruct() {

	}
	
	/**
	 * getFolder()
	 * @return string full path to voicemails folders
	 */
	public function getFolder() {
		return $this->pathFolder;
	}
	
	/**
	 * setFolder(new_folder)
	 * @param string new_folder
	 */
	public function setFolder($newPathFolder) {
		$this->pathFolder = $newPathFolder;
	}
	
	/**
	* load()
	*/
	public function load() {
		if( ( $dir = opendir($this->pathFolder) ) == FALSE ) {
			echo "<error>Can't open '". $this->pathFolder ."'</error>\n";
			return;
		}

		/** Reads all different voicemail folders (as INBOX, friends, etc.) */
		while( ( $folder = readdir($dir) ) !== false ) {

			/** If current file is a directory and not "." or ".." or "tmp" or "temp" (lowercase folders) */
			if( is_dir($this->pathFolder."/".$folder) && $folder != "." && $folder != ".." && $folder == ucfirst($folder) ) {
				$foldArr = new VoicemailFolder($folder);
				if( ( $vmFolder = @opendir($this->pathFolder ."/". $folder) ) == FALSE ) {
					echo "<error>Can't open \"". $this->pathFolder ."/". $folder ."\"</error>\n";
					continue;
				}

				/** Deletes '.' and '..' from folder list */
				$files = @scandir($this->pathFolder ."/". $folder);
				$files = @array_reverse($files);
				@array_pop($files);
				@array_pop($files);
				$files = @array_reverse($files);
				
				/** Reads into voicemail folder */
				while( ( $file = @readdir($vmFolder) ) !== false ) {
				
					/** If current file is not a directory */
					if( $file != "." && $file != ".." ) {
					
						/** Checks if current file is a TXT file */
						$arr = explode(".", $file);
						if( $arr[count($arr)-1] == "txt" ) {
							/** Reads informations in this file to create a new Voicemail */
							$fd = @fopen($this->pathFolder."/".$folder."/".$file, "r");
							
							if( $fd ) {
								$vm = new Voicemail(substr($file, 0, strrpos($file, ".")));
								$vm->setValue("id", substr($fi, 3, 4));
								while( ! feof($fd) ) {
									$buf = fgets($fd, 4096);
									$line = trim($buf);
									$pattern  = '/^(.+)=(.*)$/';
									if( preg_match($pattern, $line, $matches) ) {
										$vm->setValue($matches[1], $matches[2]);
									}
								}
								/** Adds the new Voicemail to the VoicemailFolder */
								$foldArr->addVMail($vm);
								fclose($fd);
								$tmp = $files;
								/** Adds sound files to the Voicemail */
								foreach( $tmp as $fi ) {
									if( strcmp(substr($fi, 3, 4), substr($file, 3, 4)) == 0 && strcmp(substr($fi, -3, 3), "txt") != 0 ) {
										$vm->addFormat($fi);
									}
								}
							} // end if opened
							else {
								echo "<error>Can't open ". $this->pathFolder."/".$folder."/".$file ."</error>";
							}
						} // end if text message

					} // end if not a directory
				} // end while 2
				array_push($this->lstFolders, $foldArr);
				@closedir($vmFolder);
			
			} // end if is a directory but neither "." nor ".."
		} // end while 1
		@closedir($dir);
		sort($this->lstFolders);
		return $this->lstFolders;
	}
	
	
	public function reload() {
		
	}
	
	/**
	 * getSound(a_voicemail_folder, a_voicemail_name)
	 * @param string a_voicemail_folder
	 * @param string a_voicemail_name
	 * @return bool TRUE if the voicemail exists and has sound file(s), FALSE otherwise
	 */
	public function getSound($folder, $voicemail) {
		foreach( $this->lstFolders as $fol ) {	
			if( strcmp($fol->getName(), $folder) == 0 ) {
				$vm = $fol->getVMByName($voicemail);
				if( $vm != NULL ) {
					$arr = $vm->getFormats();
					foreach( $arr as $file ) {
						$basename = substr($file, strrpos($file, ".")+1, strlen($file));
						echo "<sound folder=\"". $folder ."\" file=\"". $voicemail ."\" format=\"". $basename ."\">";
						//echo "<a href=\"". $this->pathFolder ."/". $folder ."/". $file ."\">$file</a>";
						$handle = fopen($this->pathFolder ."/". $folder ."/". $file, "r");
						$contents = fread($handle, filesize($this->pathFolder ."/". $folder ."/". $file));
						fclose($handle);
						echo base64_encode($contents);
						echo "</sound>\n";
					}
					return TRUE;
				}
			}
		}
		echo "<error-message>No file found</error-message>\n";
		return FALSE;
	}
	
	/**
	* rename(a_voicemail_folder, a_voicemail_old_name, a_voicemail_new_name)
	* @param string a_voicemail_folder
	* @param string a_voicemail_old_name
	* @param string a_voicemail_new_name
	* @return bool TRUE if the all files (TXT+sound) have been well renamed, FALSE otherwise
	*/
	public function rename($folderName, $oldName, $newName) {
		foreach( $this->lstFolders as $fol ) {	
			if( strcmp($fol->getName(), $folderName) == 0 ) {
				$vm = $fol->getVMByName($oldName);
				if( $vm != NULL ) {
					if( ! rename($this->pathFolder ."/". $folderName ."/". $oldName .".txt", $this->pathFolder ."/". $folderName ."/". $newName .".txt") ) {
						echo "<error-message>". $oldname ." can't be renamed</error-message>";
					}
					$arr = $vm->getFormats();
					foreach( $arr as $file ) {
						$basename = substr($file, strrpos($file, ".")+1, strlen($file));
						$old = $this->pathFolder ."/". $folderName ."/". $oldName .".". $basename;
						$new = $this->pathFolder ."/". $folderName ."/". $newName .".". $basename;
						if( ! rename($old, $new) ) {
							echo "<error-message>". $old ." can't be renamed</error-message>";
						}
					}
					$fol->rename($oldName, $newName);
					return TRUE;
				}
			}
		}
		echo "<error-message>Voicemail ". $oldName ." not found</error-message>";
		return FALSE;
	}
	
	/**
	* deleteFolder(a_voicemail_folder)
	* @param string a_voicemail_folder
	* @return bool
	*/
	public function deleteFolder( $folder ) {
		foreach( $this->lstFolders as $fol ) {	
			if( strcmp($fol->getName(), $folder) == 0 ) {
				foreach( $fol->getListVM() as $vm ) {
					if( ! unlink($this->pathFolder ."/". $folder ."/". $vmName .".txt") ) {
						echo "<error-message>". $vm ." wasn't deleted</error-message>";
					}
					$arr = $vm->getFormats();
					foreach( $arr as $file ) {
						$basename = substr($file, strrpos($file, ".")+1, strlen($file) );
						if( ! unlink($this->pathFolder ."/". $folder ."/". $vmName .".". $basename) ) {
							echo "<error-message>". $file ." wasn't deleted</error-message>";
						}
					}
				}
				return TRUE;
			}
		}
		echo "<error-message>File ". $vmName ." not found</error-message>";
		return FALSE;
	}
	
	/**
	* delete(a_voicemail_folderm, a_voicemail)
	* @param string a_voicemail_folder
	* @param string a_voicemail_name
	* @return bool TRUE if all files (TXT+sound) have been well deleted, FALSE otherwise
	*/
	public function delete($folder, $vmName) {
		foreach( $this->lstFolders as $fol ) {	
			if( strcmp($fol->getName(), $folder) == 0 ) {
				$vm = $fol->getVMByName($vmName);
				if( $vm != NULL ) {
					if( ! unlink($this->pathFolder ."/". $folder ."/". $vmName .".txt") ) {
						echo "<error-message>". $vm ." wasn't deleted</error-message>";
					}
					$arr = $vm->getFormats();
					foreach( $arr as $file ) {
						$basename = substr($file, strrpos($file, ".")+1, strlen($file));
						if( ! unlink($this->pathFolder ."/". $folder ."/". $vmName .".". $basename) ) {
							echo "<error-message>". $file ." wasn't deleted</error-message>";
						}
					}
					return TRUE;
				}
			}
		}
		echo "<error-message>File ". $vmName ." not found</error-message>";
		return FALSE;
	}

} // end of VMStorageFile
?>
