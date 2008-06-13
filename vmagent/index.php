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

if( ! isset( $_SERVER['PHP_AUTH_USER'] ) ) {
	header("WWW-Authenticate: Basic realm=\"VoiceMail Access\"");
	header("HTTP/1.0 401 Unauthorized");
	exit;
}

// http:// <vm-context> - <vm-number> : <vm-password> @ <asterisk-addr> / <path/to/web/folder/with/slashs> / <command>

// Description
// -----------
// http://
// vm-context  -->  voicemail context
// -
// vm-number  -->  voicemail number, as declared in voicemail.conf
// :
// vm-password  -->  voicemail password, as defined in voicemail.conf
// @
// asterisk-addr  -->  ip address or domain name (127.0.0.1 or localhost) target to asterisk server
// /
// path/to/web/folder/with/slashs  -->  full path to web service folder (/var/www/web-service/action/job will be web-service/action/job)
// /
// command  -->  the right command to execute (eg. get, list, delete, etc.)
//          -->  possible values :
//                  - list   : index/list
//                  - sound  : index/sound?f=<FOLDER_NAME>&v=<VOICEMAIL_NAME>
//                  - del    : index/del?f=<FOLDER_NAME>&v=<VOICEMAIL_NAME>
//                  - rename : index/rename?f=<FOLDER_NAME>&o=<VOICEMAIL_OLD_NAME>&n=<VOICEMAIL_NEW_NAME>
//                  - 



// http://default-6666:735@127.0.0.1/uml/


header("Content-Type: text/xml; $vmencoding"); 
echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
echo "<result>";

require_once "VMAgent.php";
	
$agent = new VMAgent("file", "file");


/***********************************************************
 * LOGIN
 **********************************************************/
// Gets context, login and password from url
$context = substr( $_SERVER['PHP_AUTH_USER'] ,
				   0 ,
				   strpos( $_SERVER['PHP_AUTH_USER'] , "-" ) );
$login   = substr( $_SERVER['PHP_AUTH_USER'] ,
				   strpos( $_SERVER['PHP_AUTH_USER'] , "-" ) + 1 ,
				   strlen( $_SERVER['PHP_AUTH_USER'] ) );
$pass    = $_SERVER['PHP_AUTH_PW'];
//echo "\n    <context>". $context ."</context>";
//echo "\n    <user>". $login ."</user>";
//echo "\n    <pass>". $pass ."</pass>\n";
$is_logged = $agent->login( $login , $pass , $context );
//echo "\n    <check>";
if( $is_logged == FALSE ) {
	echo "  <login>ERROR</login>\n";
}


/***********************************************************
 * ACTION
 **********************************************************/
if( $is_logged ) {
	$agent->load();
	if( isset( $_SERVER['PATH_INFO'] ) ) {
		$file = substr( $_SERVER['PATH_INFO'] , 1 );
//		echo "  <include>". $file ."</include>\n";
		if( file_exists( $file ) ) {
//			echo "file exists !\n";
			include( $file );//.".php";
		}
	}
}


/***********************************************************
 * LOGOUT
 **********************************************************/
if( $agent->logout() == FALSE ) {
	echo "  <logout>KO</logout>\n";
}

echo "</result>";

?>
