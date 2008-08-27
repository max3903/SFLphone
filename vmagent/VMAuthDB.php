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

require_once 'VMAuth.php';

/**
 * class VMAuthDB
 * 
 */
class VMAuthDB extends VMAuth {
	
	/** Private vars */
	private $astDir;
	private $DBtype;
	private $DBbase;
	private $DBtable = "voicemail";
	
	private $DBconnect;
	private $DBquery;
	private $DBclose;
	
	private $DBuser;
	private $DBpass;
	
	private $DBhost;
	private $DBport;
	private $DBsock;
	
	/**
	* Constructor
	* @param string context
	* @param string login
	* @param string pass
	*/
	public function __construct($login, $pass, $context, $astDir) {
		parent::__construct($login, $pass, $context);
		$this->astDir = $astDir;
		$this->autoConfig();
		$this->initDB();
	}
	
	/**
	* Destructor
	*/
	public function __destruct() {
		
	}
	
	private function autoConfig() {
		exec("sudo -i");
		$fd = @fopen($this->astDir."/extconfig.conf", "r");
		if( !$fd ) {
			echo "<error>";
			echo "Could not open the EXTCONFIG_CONF file : ". $this->astDir ."/extconfig.conf"; 
			echo "</error>";
			return FALSE;
		}
		
		$valid = FALSE;
		$cont = "";
		/* Get informations about database (type, name and table */
		$pattern = '/^\s*voicemail.conf\s*=>\s*(.*),(.*),(.*)/';
		while( ! feof($fd) ) {
			$buf = fgets($fd, 4096);
			$line = trim($buf);
			if( preg_match($pattern, $line, $matches) ) {
				$this->DBtype  = $matches[1];
				$this->DBbase  = $matches[2];
				$this->DBtable = $matches[3];
				echo "<DBautoConfig>";
				echo "<DBtype>".  $this->DBtype ."</DBtype>";
				echo "<DBbase>".  $this->DBbase ."</DBbase>";
				echo "<DBtable>". $this->DBtable ."</DBtable>";
				echo "</DBautoConfig>";
			}
		}
		fclose($fd);

		return $valid;
	}
	
	private function initDB() {
		echo "<iniDB>";
		echo "<DBtype>";
		switch( $this->DBtype ) {
			case "mysql" :
				echo "mysql";
				$this->DBconnect = "mysql_connect";
				$this->DBselect  = "mysql_select_db";
				$this->DBquery   = "mysql_query";
				$this->DBclose   = "mysql_close";
				$this->connectMysql();
				break;
			case "pgsql" :
				echo "pgsql";
				$this->DBconnect = "pg_connect";
				$this->DBselect  = $this->DBconnect;
				$this->DBquery   = "pg_query";
				$this->DBclose   = "pg_close";
				break;
			case "odbc" :
				case "odbc";
				$this->DBconnect = "";
				$this->DBselect  = "";
				$this->DBquery   = "";
				$this->DBclose   = "";
				break;
			default :
				echo "default";
				$this->DBconnect = "";
				$this->DBselect  = "";
				$this->DBquery   = "";
				$this->DBclose   = "";
				break;
		}
		echo "</DBtype>";
		echo "<DBconnect>".$this->DBconnect ."</DBconnect>";
		echo "<DBselect>". $this->DBselect ."</DBselect>";
		echo "<DBquery>".  $this->DBquery ."</DBquery>";
		echo "<DBclose>".  $this->DBclose ."</DBclose>";
		echo "</iniDB>";
	}
	
	/**
	* login()
	* @return bool
	*/
	public function login() {
		$con = $this->DBconnect;
		echo $this->DBconnect ."(". $this->DBhost .",". $this->DBuser. ",". $this->DBpass .")";
		if( $db = $con($this->DBhost, $this->DBuser, $this->DBpass) ) {
			$sel = $this->DBselect;
			if( $sel = @$sel($this->DBbase) ) {
				$que = $this->DBquery;
/*				echo "<req>SELECT * FROM `". $this->DBtable ."` "
					."WHERE filename='voicemail.conf' "
					."AND category='". $this->context ."' "
					."AND var_name='". $this->login ."' "
					."AND var_val REGEXP '^\\s*". $this->pass ."\\s*,';</req>";*/
				$req = $que("SELECT * FROM `". $this->DBtable ."` "
							."WHERE filename='voicemail.conf' "
							."AND category='". $this->context ."' "
							."AND var_name='". $this->login ."' "
							."AND var_val REGEXP '^\\s*". $this->pass ."\\s*,';");
				if( $req ) {
					if( mysql_num_rows($req) != 0 ) {
						return TRUE;
					} else {
						echo "<error>User not found</error>";
						return FALSE;
					}
				} else {
					echo "<error>Mysql error : ". mysql_error() ."</error>";
				}
			} else {
				echo "<error>Can't connect to voicemail mysql table</error>";
			}
		} else {
			echo "<error>Can't connect to mysql database</error>";
		}
	}

	/**
	* logoff()
	* @return bool
	*/
	public function logoff() {
		$clo = $this->DBclose;
		@$clo();
	}
	
	private function connectMysql() {
		echo "<mysqlConfig>";
		$fd = @fopen($this->astDir."/res_mysql.conf", "r");
		if( !$fd ) {
			echo "<error>";
			echo "Could not open the RES_MYSQL_CONF file : $this->astDir/res_mysql.conf"; 
			echo "</error>";
			return FALSE;
		}
		
		$valid = FALSE;
		$cont = "";
		/* Get informations about database (type, name and table */
		$patternHost = "/^\s*dbhost\s*=\s*(.*)$/";
//		$patternName = "/^\s*dbname\s*=\s*(.*)$/";
		$patternUser = "/^\s*dbuser\s*=\s*(.*)$/";
		$patternPass = "/^\s*dbpass\s*=\s*(.*)$/";
		$patternPort = "/^\s*dbport\s*=\s*(.*)$/";
		$patternSock = "/^\s*dbsock\s*=\s*(.*)$/";
		while( ! feof($fd) ) {
			$buf = fgets($fd, 4096);
			$line = trim($buf);
			if( preg_match($patternHost, $line, $matches) ) {
				$this->DBhost = $matches[1];
			}
			if( preg_match($patternUser, $line, $matches) ) {
				$this->DBuser = $matches[1];
			}
			if( preg_match($patternPass, $line, $matches) ) {
				$this->DBpass = $matches[1];
			}
			if( preg_match($patternPort, $line, $matches) ) {
				$this->DBport = $matches[1];
			}
			if( preg_match($patternSock, $line, $matches) ) {
				$this->DBsock = $matches[1];
			}
		}
		fclose($fd);
		
		echo "<host>". $this->DBhost ."</host>";
		echo "<user>". $this->DBuser ."</user>";
		echo "<pass>". $this->DBpass ."</pass>";
		echo "<port>". $this->DBport ."</port>";
		echo "<sock>". $this->DBsock ."</sock>";
		echo "</mysqlConfig>";
		return $valid;
	}

} // end of VMAuthDB



header("Content-Type: text/xml; UTF-8"); 
echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";	
echo "<result>";
$vmauth = new VMAuthDB("6666", "735", "default", "/etc/asterisk");
$vmauth->login();
$vmauth->logoff();

echo "</result>"

?>
