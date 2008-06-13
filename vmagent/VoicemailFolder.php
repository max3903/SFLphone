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

/**
 * class VoicemailFolder
 * 
 */
class VoicemailFolder {

	/** Private instance vars */
	private $folderName;
	private $lstVM;
	
	/**
	 * VoicemailFolder -- Constructor
	 */	
	public function __construct( $name="" ) {
		$this->folderName = $name;
		$this->lstVM = array();
	}
	
	/**
	 * VoicemailFolder -- Destructor
	 */	
	public function __destruct() {
		for( $i=count($this->lstVM) ; $i>=0 ; $i-- ) {
			array_pop( $this->lstVM );
		}
	}
	
	/**
	 * setName(name)
	 * @param string name
	 */
	public function setName( $name ) {
		$this->$folderName = $name;
	}
	
	/**
	 * getName()
	 * @return string folderName
	 */
	public function getName() {
		return $this->folderName;
	}
	
	/**
	 * getCount()
	 * @return int nb_of_voicemail
	 */
	public function getCount() {
		return count( $this->lstVM );
	}	

	/**
	 * getListVM()
	 * @return array<Voicemail> list_of_voicemail
	 */
	public function getListVM() {
		return $this->lstVM;
	}
	
	/**
	 * getVMByName(a_voicemail_name)
	 * @param string a_voicemail_name
	 * @return Voicemail the_searched_voicemail
	 */
	public function getVMByName( $newName ) {
		foreach( $this->getListVM() as $vm ) {
			if( strcmp( $vm->getName() , $newName ) == 0 ) {
				return $vm;
			}
		}
		return NULL;
	}
	
	/**
	 * addVMail(a_voicemail)
	 * @param Voicemail a_voicemail
	 */
	public function addVMail( $vm ) {
		array_push( $this->lstVM , $vm );
	}
	
	/**
	 * rename( old_vm_name , new_vm_name )
	 * @param string old_vm_name
	 * @param string new_vm_name
	 * @return string result : "OK" if ok, a text message otherwise
	 */
	public function rename( $oldName , $newName ) {
		foreach( $this->getListVM() as $vm ) {
			if( strcmp( $vm->getName() , $oldName ) == 0 ) {
				$vm->setName( $newName );
				foreach( $vm->getFormats() as $format ) {
					$arr = explode( "." , $format );
					$new = $newName .".". $arr[ count($arr)-1 ];
				}
				return "OK";
			}
		}
		return "Didn't find folder";
	}
	
	/**
	 * toShortString()
	 */
	public function toShortString() {
		$ret = "  <directory name=\"". $this->getName() ."\" count=\"". $this->getCount() ."\">";
		if( $this->getCount() != 0 ) {
			$ret .= "\n";
			foreach( $this->lstVM as $vm ) {
				$ret .= $vm->toShortString();
			}
			$ret .= "  ";
		}
		$ret .= "</directory>\n";
		return $ret;
	}
	
	/**
	 * toString()
	 */
	public function toString() {
		$ret = "<directory name=\"". $this->getName() ."\" count=\"". $this->getCount() ."\">";
		if( $this->getCount() != 0 ) {
//			$ret .= "\n";
			foreach( $this->lstVM as $vm ) {
				$ret .= $vm->toString();
			}
//			$ret .= "  ";
		}
		$ret .= "</directory>";
		return $ret;
	}

} // end of Voicemail
?>
