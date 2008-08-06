<?php
/*
 *  Copyright (C) 2008 Savoir-Faire Linux inc.
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
 * class Voicemail
 * 
 */
class Voicemail {

	/** Private instance vars */
	private $callerchan;
	private $callerid;
	private $category;
	private $context;
	private $duration;
	private $exten;
	private $id;
	private $macrocontext;
	private $name;
	private $origmailbox;
	private $origdate;
	private $origtime;
	private $priority;

	private $lstFormat;

	

	/**
	 * Voicemail -- Constructor
	 */	
	public function __construct($name="") {
		$this->name = $name;
		$this->lstFormat = array();
	}
	
	/**
	 * Voicemail -- Destructor
	 */	
	public function __destruct() {
		
	}
	
	/**
	 * getName()
	 * @return string name
	 */
	public function getName() {
		return $this->name;
	}
	
	/**
	 * setName( a_name )
	 * @param string a_name
	 */
	public function setName($nam) {
		$this->name = $nam;
	}
	
	/**
	 * setValue(name,value)
	 * @param string name
	 * @param string value
	 */
	public function setValue($key, $value) {
		//$this->tab[ $name ] = $value;
		//echo count( $this->tab )."<br/>";
		$this->$key = $value;
	} // end of member function setValue
	
	/**
	 * getFormats()
	 * @return array<string> list_of_formats
	 */
	public function getFormats() {
		return $this->lstFormat;
	}
	
	/**
	 * addFormat(a_sound_format)
	 * @param string a_sound_format
	 */
	public function addFormat($format) {
		array_push($this->lstFormat, $format);
	}
	
	/**
	 * toString()
	 * @return string result
	 */
	public function toString() {
		$ret  = "<voicemail>";
		$ret .= "<callerchan>". $this->callerchan ."</callerchan>";
		$ret .= "<callerid>". $this->callerid ."</callerid>";
		$ret .= "<category>". $this->category ."</category>";
		$ret .= "<context>". $this->context ."</context>";
		$ret .= "<duration>". $this->duration ."</duration>";
		$ret .= "<exten>". $this->exten ."</exten>";
		$ret .= "<id>". $this->id ."</id>";
		$ret .= "<macrocontext>". $this->macrocontext ."</macrocontext>";
		$ret .= "<name>". $this->name ."</name>";
		$ret .= "<origdate>". $this->origdate ."</origdate>";
		$ret .= "<origmailbox>". $this->origmailbox ."</origmailbox>";
		$ret .= "<origtime>". $this->origtime ."</origtime>";
		$ret .= "<priority>". $this->priority ."</priority>";

/*		foreach( $this as $key => $val ) {
			$ret .= "<$key>$val</$key>";
		}*/
		foreach( $this->lstFormat as $val ) {
			$ret .= "<format>$val</format>";
		}
		$ret .= "</voicemail>";
		return $ret;
	}

} // end of Voicemail
?>
