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

/**
 * class VMAuth
 * 
 */
abstract class VMAuth {

	/** Protected instance vars */
	protected $context;
	protected $login;
	protected $pass;

	/**
	* VMAuth -- Constructor
	* @param string context
	* @param string login
	* @param string pass
	*/
	/*
	public function __construct( $context , $login ,  $pass ) {
		$this->context = $context;
		$this->login = $login;
		$this->pass = $pass;
	}
	*/
	
	/**
	* VMAuth -- Destructor
	*/
	public function __destruct() {
		
	}

	/**
	* login()
	* must be defined in inherited classes 
	* @return bool
	*/
	abstract public function login();

	/**
	* logoff()
	* ;ust be defined in inherited classes
	* @return bool
	*/
	abstract public function logoff();

} // end of VMAuth
?>
