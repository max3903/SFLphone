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
//					- one folder         : index/<FOLDER_NAME>
//					- one voicemail      : index/<FOLDER_NAME>/<VOICEMAIL_NAME>
//					- list (all folders) : index  OR  index/list
//					- sound              : index/<FOLDER_NAME>/<VOICEMAIL_NAME>/sound
//					- del                : index/<FOLDER_NAME>/<VOICEMAIL_NAME>/del
//					- rename             : index/<FOLDER_NAME>/<VOICEMAIL_OLD_NAME>/rename?n=<VOICEMAIL_NEW_NAME>
//					- 


// http://default-6666:735@127.0.0.1/uml/

header("Content-Type: text/xml; UTF-8"); 
echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
echo "<result>";

require_once "VMAgent.php";

/***********************************************************
 * INIT & LOGIN
 **********************************************************/
// Gets context, login and password from url
$context = substr( $_SERVER['PHP_AUTH_USER'] ,
				   0 ,
				   strpos( $_SERVER['PHP_AUTH_USER'] , "-" ) );
$login   = substr( $_SERVER['PHP_AUTH_USER'] ,
				   strpos( $_SERVER['PHP_AUTH_USER'] , "-" ) + 1 ,
				   strlen( $_SERVER['PHP_AUTH_USER'] ) );
$pass    = $_SERVER['PHP_AUTH_PW'];

$agent = new VMAgent($login, $pass, $context);
$is_logged = $agent->login( $login , $pass , $context );

if( $is_logged == FALSE ) {
	echo "<error>Login or password incorrect, try again</error>\n";
}


/***********************************************************
 * ACTION
 **********************************************************/
if( $is_logged ) {
	$agent->load();
	if( ! empty( $_SERVER['PATH_INFO'] ) && $_SERVER['PATH_INFO'] != "/" ) {
		$path = trim( rtrim( $_SERVER['PATH_INFO'] , "/" ) , "/" );
		$arr = explode( "/" , $path );
		/** rename, list, del, sound */
		if( file_exists( $arr[count($arr)-1] ) ) {
			include( $arr[count($arr)-1] );
		} else {
			/** Gets Voicemail Folder */
			if( count($arr) == 1 ) {
				echo $agent->getVMFByName( $arr[ count($arr)-1 ] )->toString();
			} else { /** Gets Voicemail */
				echo "<directory name=\"". $arr[ count($arr)-2 ] ."\">";
				echo $agent->getVMByName( $arr[ count($arr)-2 ] , $arr[ count($arr)-1 ] )->toString();
				echo "</directory>";
			}
		}
	} else {
		include( "list" );
	}
}


/***********************************************************
 * LOGOUT
 **********************************************************/
if( $agent->logout() == FALSE ) {
	echo "<logout>KO</logout>\n";
}

echo "</result>";

?>
