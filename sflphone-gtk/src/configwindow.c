/*
 *  Copyright (C) 2007 Savoir-Faire Linux inc.
 *  Author: Pierre-Luc Beaudoin <pierre-luc.beaudoin@savoirfairelinux.com>
 *  Author: Emmanuel Milou <emmanuel.milou@savoirfairelinux.com>
 *  Author: Guillaume Carmel-Archambault <guillaume.carmel-archambault@savoirfairelinux.com>
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

#include <accountlist.h>
#include <accountwindow.h>
#include <actions.h>
#include <calltree.h>
#include <config.h>
#include <dbus.h>
#include <mainwindow.h>
#include <glwidget.h>

#include <stdlib.h>
#include <stdio.h>

#include <gtk/gtk.h>

gboolean dialogOpen = FALSE;
gboolean ringtoneEnabled = TRUE;
gint webcamIndex = 0;

GtkListStore *accountStore;
GtkWidget *codecTreeView;		// View used instead of store to get access to selection
GtkWidget *videoCodecTreeView;	// instead of keeping selected codec as a variable

GtkListStore *inputAudioPluginStore;
GtkListStore *outputAudioPluginStore;
GtkListStore *outputAudioDeviceManagerStore;
GtkListStore *inputAudioDeviceManagerStore;
GtkListStore *webcamDeviceStore;
GtkListStore *resolutionStore;
GtkListStore *bitrateStore;

GtkWidget *addButton;
GtkWidget *editButton;
GtkWidget *deleteButton;
GtkWidget *restoreButton;
GtkWidget *accountMoveDownButton;
GtkWidget *accountMoveUpButton;

GtkWidget *outputDeviceComboBox;
GtkWidget *inputDeviceComboBox;
GtkWidget *pluginComboBox;
GtkWidget *webcamDeviceComboBox;
GtkWidget *resolutionComboBox;
GtkWidget *bitrateComboBox;

GtkWidget *moveUpButton;
GtkWidget *moveDownButton;
GtkWidget *moveUpButtonVideo;

GtkWidget *moveDownButtonVideo; 

GtkDialog * dialog;
GtkWidget * notebook;
GtkObject *brightnessAdjustment, *contrastAdjustment, *colourAdjustment;

GtkWidget *activateCheckBox;
GtkWidget *cancelCheckBox;
gboolean enableStatus;
gboolean disableStatus;

GtkWidget *moveDownButtonVideo;
GtkWidget *codecMoveUpButton;
GtkWidget *codecMoveDownButton;

GtkWidget* status;

account_t *selectedAccount;


// Account properties
enum {
	COLUMN_ACCOUNT_ALIAS,
	COLUMN_ACCOUNT_TYPE,
	COLUMN_ACCOUNT_STATUS,
	COLUMN_ACCOUNT_ACTIVE,
	COLUMN_ACCOUNT_DATA,
	COLUMN_ACCOUNT_COUNT
};


// Codec properties ID
enum {
	COLUMN_CODEC_ACTIVE,
	COLUMN_CODEC_NAME,
	COLUMN_CODEC_FREQUENCY,
	COLUMN_CODEC_BITRATE,
	COLUMN_CODEC_BANDWIDTH,
	CODEC_COLUMN_COUNT
};

/**
 * Fills the treelist with accounts
 */
void
config_window_fill_account_list()
{
	if(dialogOpen)
	{
		GtkTreeIter iter;

		gtk_list_store_clear(accountStore);
		int i;
		for(i = 0; i < account_list_get_size(); i++)
		{
			account_t * a = account_list_get_nth (i);
			if (a)
			{
			  g_print("fill account list : %s\n" , (gchar*)g_hash_table_lookup(a->properties, ACCOUNT_ENABLED));
				gtk_list_store_append (accountStore, &iter);
				gtk_list_store_set(accountStore, &iter,
						COLUMN_ACCOUNT_ALIAS, g_hash_table_lookup(a->properties, ACCOUNT_ALIAS),  // Name
						COLUMN_ACCOUNT_TYPE, g_hash_table_lookup(a->properties, ACCOUNT_TYPE),   // Protocol
						COLUMN_ACCOUNT_STATUS, account_state_name(a->state),      // Status
						COLUMN_ACCOUNT_ACTIVE, (g_strcasecmp(g_hash_table_lookup(a->properties, ACCOUNT_ENABLED),"TRUE") == 0)? TRUE:FALSE,  // Enable/Disable
						COLUMN_ACCOUNT_DATA, a,   // Pointer
						-1);
			}
		}

		gtk_widget_set_sensitive( GTK_WIDGET(editButton),   FALSE);
		gtk_widget_set_sensitive( GTK_WIDGET(deleteButton), FALSE);
	}
}

/**
 * Fills the tree list with supported audio codecs
 */
void
config_window_fill_codec_list()
{
	if(dialogOpen)
	{
		GtkListStore *codecStore;
		GtkTreeIter iter;
		
		// Get model of view and clear it
		codecStore = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(codecTreeView)));
		gtk_list_store_clear(codecStore);

		// Insert codecs
		int i;
		for(i = 0; i < codec_list_get_size(); i++)
		{
			codec_t *c = codec_list_get_nth(i);
			printf("%s\n", c->name);
			if(c)
			{
				gtk_list_store_append(codecStore, &iter);
				gtk_list_store_set(codecStore, &iter,
						COLUMN_CODEC_ACTIVE,	c->is_active,									// Active
						COLUMN_CODEC_NAME,		c->name,										// Name
						COLUMN_CODEC_FREQUENCY,	g_strdup_printf("%d kHz", c->sample_rate/1000),	// Frequency (kHz)
						COLUMN_CODEC_BITRATE,	g_strdup_printf("%.1f kbps", c->_bitrate),		// Bitrate (kbps)
						COLUMN_CODEC_BANDWIDTH,	g_strdup_printf("%.1f kbps", c->_bandwidth),	// Bandwidth (kpbs)
						-1);
			}
		}
	}
}

/**
 * Fills the tree list with supported video codecs
 */
void
config_window_fill_video_codec_list()
{
	if(dialogOpen)
	{
		GtkListStore *videoCodecStore;
		GtkTreeIter iter;
		
		// Get model of view and clear it
		videoCodecStore = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(videoCodecTreeView)));
		gtk_list_store_clear(videoCodecStore);

		// Insert codecs
		int i;
		for(i = 0; i < video_codec_list_get_size(); i++)
		{
			videoCodec_t *c = video_codec_list_get_nth(i);
			printf("%s\n", c->name);
			if(c)
			{
				gtk_list_store_append(videoCodecStore, &iter);
				gtk_list_store_set(videoCodecStore, &iter,
						COLUMN_CODEC_ACTIVE,	c->is_active,									// Active
						COLUMN_CODEC_NAME,		c->name,										// Name
						-1);
			}
		}
	}
}

/**
 * Fill store with input audio plugins
 */
void
config_window_fill_input_audio_plugin_list()
{
	GtkTreeIter iter;
	gchar** list;
	gchar* managerName;

	gtk_list_store_clear(inputAudioPluginStore);
	
	// Call dbus to retreive list
	list = dbus_get_input_audio_plugin_list();
	
	// For each API name included in list
	int c = 0;
	for(managerName = list[c]; managerName != NULL; managerName = list[c])
	{
		c++;
		gtk_list_store_append(inputAudioPluginStore, &iter);
		gtk_list_store_set(inputAudioPluginStore, &iter, 0 , managerName, -1);
	}
}

/**
 * Fill store with output audio plugins
 */
void
config_window_fill_output_audio_plugin_list()
{
	GtkTreeIter iter;
	gchar** list;
	gchar* managerName;

	gtk_list_store_clear(outputAudioPluginStore);
	
	// Call dbus to retreive list
	list = dbus_get_output_audio_plugin_list();
	
	// For each API name included in list
	int c = 0;
	for(managerName = list[c]; managerName != NULL; managerName = list[c])
	{
		c++;
		gtk_list_store_append(outputAudioPluginStore, &iter);
		gtk_list_store_set(outputAudioPluginStore, &iter, 0 , managerName, -1);
	}
}
/**
 * Fill output audio device store
 */
void
config_window_fill_output_audio_device_list()
{
	GtkTreeIter iter;
	gchar** list;
	gchar** audioDevice;
	int index;

	gtk_list_store_clear(outputAudioDeviceManagerStore);
	
	// Call dbus to retreive list
	list = dbus_get_audio_output_device_list();
	
	// For each device name included in list
	int c = 0;
	for(audioDevice = list; *list ; list++)
	{
		index = dbus_get_audio_device_index( *list );
		gtk_list_store_append(outputAudioDeviceManagerStore, &iter);
		gtk_list_store_set(outputAudioDeviceManagerStore, &iter, 0, *list, 1, index, -1);
		c++;
	}
}

/**
 * Select active output audio device
 */
void
select_active_output_audio_device()
{
	GtkTreeModel* model;
	GtkTreeIter iter;
	gchar** devices;
	int currentDeviceIndex;
	int deviceIndex;

	// Select active output device on server
	devices = dbus_get_current_audio_devices_index();
	currentDeviceIndex = atoi(devices[0]);
	printf(_("audio device index for output = %d\n"), currentDeviceIndex);
	model = gtk_combo_box_get_model(GTK_COMBO_BOX(outputDeviceComboBox));
	
	// Find the currently set output device
	gtk_tree_model_get_iter_first(model, &iter);
	do {
		gtk_tree_model_get(model, &iter, 1, &deviceIndex, -1);
		if(deviceIndex == currentDeviceIndex)
		{
			// Set current iteration the active one
			gtk_combo_box_set_active_iter(GTK_COMBO_BOX(outputDeviceComboBox), &iter);
			return;
		}
	} while(gtk_tree_model_iter_next(model, &iter));

	// No index was found, select first one
	g_print("Warning : No active output device found");
	gtk_combo_box_set_active(GTK_COMBO_BOX(outputDeviceComboBox), 0);
}

/**
 * Fill input audio device store
 */
void
config_window_fill_input_audio_device_list()
{
	GtkTreeIter iter;
	gchar** list;
	gchar** audioDevice;
	int index ;
	gtk_list_store_clear(inputAudioDeviceManagerStore);
	
	// Call dbus to retreive list
	list = dbus_get_audio_input_device_list();
	
	// For each device name included in list
	//int c = 0;
	for(audioDevice = list; *list; list++)
	{
		index = dbus_get_audio_device_index( *list );
		gtk_list_store_append(inputAudioDeviceManagerStore, &iter);
		gtk_list_store_set(inputAudioDeviceManagerStore, &iter, 0, *list, 1, index, -1);
		//c++;
	}
}

/**
 * Select active input audio device
 */
void
select_active_input_audio_device()
{
	GtkTreeModel* model;
	GtkTreeIter iter;
	gchar** devices;
	int currentDeviceIndex;
	int deviceIndex;

	// Select active input device on server
	devices = dbus_get_current_audio_devices_index();
	currentDeviceIndex = atoi(devices[1]);
	model = gtk_combo_box_get_model(GTK_COMBO_BOX(inputDeviceComboBox));
	
	// Find the currently set input device
	gtk_tree_model_get_iter_first(model, &iter);
	do {
		gtk_tree_model_get(model, &iter, 1, &deviceIndex, -1);
		if(deviceIndex == currentDeviceIndex)
		{
			// Set current iteration the active one
			gtk_combo_box_set_active_iter(GTK_COMBO_BOX(inputDeviceComboBox), &iter);
			return;
		}
	} while(gtk_tree_model_iter_next(model, &iter));

	// No index was found, select first one
	g_print("Warning : No active input device found");
	gtk_combo_box_set_active(GTK_COMBO_BOX(inputDeviceComboBox), 0);
}

void
update_combo_box( gchar* plugin )
{
	// set insensitive the devices widget if the selected plugin is default
	if( g_strcasecmp( plugin , "default" ) == 0)
	{
	  gtk_widget_set_sensitive( GTK_WIDGET ( outputDeviceComboBox ) , FALSE );
	  gtk_widget_set_sensitive( GTK_WIDGET ( inputDeviceComboBox ) , FALSE );
	}
	else
	{
	  gtk_widget_set_sensitive( GTK_WIDGET ( outputDeviceComboBox ) , TRUE );
	  gtk_widget_set_sensitive( GTK_WIDGET ( inputDeviceComboBox ) , TRUE );
	}
}
/**
 * Select the output audio plugin by calling the server
 */
static void
select_output_audio_plugin(GtkComboBox* widget, gpointer data)
{
	GtkTreeModel* model;
	GtkTreeIter iter;
	int comboBoxIndex;
	gchar* pluginName;
	
	comboBoxIndex = gtk_combo_box_get_active(widget);
	
	if(comboBoxIndex >= 0)
	{
		model = gtk_combo_box_get_model(widget);
		gtk_combo_box_get_active_iter(widget, &iter);
		gtk_tree_model_get(model, &iter, 0, &pluginName, -1);	
		dbus_set_output_audio_plugin(pluginName);
		update_combo_box( pluginName);
	}
}

/**
 * Select active output audio plugin
 */
void
select_active_output_audio_plugin()
{
	GtkTreeModel* model;
	GtkTreeIter iter;
	gchar* plugin;
	gchar* tmp;

	// Select active output device on server
	plugin = dbus_get_current_audio_output_plugin();
	tmp = plugin;
	model = gtk_combo_box_get_model(GTK_COMBO_BOX(pluginComboBox));
	  
	// Find the currently alsa plugin
	gtk_tree_model_get_iter_first(model, &iter);
	do {
		gtk_tree_model_get(model, &iter, 0, &plugin , -1);
		if( g_strcasecmp( tmp , plugin ) == 0 )
		{
			// Set current iteration the active one
			gtk_combo_box_set_active_iter(GTK_COMBO_BOX(pluginComboBox), &iter);
			update_combo_box( plugin );
			return;
		}
	} while(gtk_tree_model_iter_next(model, &iter));

	// No index was found, select first one
	g_print("Warning : No active output device found\n");
	gtk_combo_box_set_active(GTK_COMBO_BOX(pluginComboBox), 0);
	update_combo_box("default");
}


/**
 * Set the audio output device on the server with its index
 */
static void
select_audio_output_device(GtkComboBox* comboBox, gpointer data)
{
	GtkTreeModel* model;
	GtkTreeIter iter;
	int comboBoxIndex;
	int deviceIndex;
	
	comboBoxIndex = gtk_combo_box_get_active(comboBox);
	
	if(comboBoxIndex >= 0)
	{
		model = gtk_combo_box_get_model(comboBox);
		gtk_combo_box_get_active_iter(comboBox, &iter);
		gtk_tree_model_get(model, &iter, 1, &deviceIndex, -1);
		
		dbus_set_audio_output_device(deviceIndex);
	}
}

/**
 * Set the audio input device on the server with its index
 */
static void
select_audio_input_device(GtkComboBox* comboBox, gpointer data)
{
	GtkTreeModel* model;
	GtkTreeIter iter;
	int comboBoxIndex;
	int deviceIndex;
	
	comboBoxIndex = gtk_combo_box_get_active(comboBox);
	
	if(comboBoxIndex >= 0)
	{
		model = gtk_combo_box_get_model(comboBox);
		gtk_combo_box_get_active_iter(comboBox, &iter);
		gtk_tree_model_get(model, &iter, 1, &deviceIndex, -1);
		
		dbus_set_audio_input_device(deviceIndex);
	}
}

/**
 * Set the webcam device on the server with its name
 */
static void
select_webcam_device(GtkComboBox* widget, gpointer data)
{
	GtkTreeModel* model;
	GtkTreeIter iter;
	gchar* name;
	
	webcamIndex = gtk_combo_box_get_active(widget);
	
	if(webcamIndex >= 0)
	{
		model = gtk_combo_box_get_model(widget);
		gtk_combo_box_get_active_iter(widget, &iter);
		gtk_tree_model_get(model, &iter, 0, &name, -1);
		dbus_disable_local_video_pref();
		dbus_set_webcam_device(name);
		if(strcmp(name, "No device")!=0)
		{	
			dbus_enable_local_video_pref();
		}
		update_notebook();
		
	}
	
	
}

/**
 * Fill webcam device store
 */
void
config_window_fill_webcam_device_list()
{
	GtkTreeIter iter;
	gchar** list;
	gchar* webcamName;
	gtk_list_store_clear(webcamDeviceStore);
	
	// Call dbus to retrieve list
	list = dbus_get_webcam_device_list();
	
	// For each webcam included in list
	int c = 0;
	
	for(webcamName = list[c]; webcamName != NULL; webcamName = list[c])
	{
		gtk_list_store_append(webcamDeviceStore, &iter);
		gtk_list_store_set(webcamDeviceStore, &iter, 0 , webcamName, -1);
		c++;
	}
	
	//Add the option to use no device
	gtk_list_store_append(webcamDeviceStore, &iter);
	gtk_list_store_set(webcamDeviceStore, &iter, 0 , "No device", -1);
}

/**
 * Select active webcam device
 */
void
select_active_webcam_device()
{
	// If none has been selected yet, select the first one
	gtk_combo_box_set_active(GTK_COMBO_BOX(webcamDeviceComboBox), webcamIndex);
}

/**
 * Set the resolution on the server with its name
 */
static void
select_resolution(GtkComboBox* widget, gpointer data)
{
	GtkTreeModel* model;
	GtkTreeIter iter;
	int comboBoxIndex;
	gchar* name;
	
	comboBoxIndex = gtk_combo_box_get_active(widget);
	
	if(comboBoxIndex >= 0)
	{
		model = gtk_combo_box_get_model(widget);
		gtk_combo_box_get_active_iter(widget, &iter);
		gtk_tree_model_get(model, &iter, 0, &name, -1);	
		dbus_set_resolution(name);
	}
}

/**
 * Fill resolution store
 */
void
config_window_fill_resolution_list()
{
	GtkTreeIter iter;
	gchar** list;
	gchar* resolutionName;
	gtk_list_store_clear(resolutionStore);
	
	// Call dbus to retrieve list
	list = dbus_get_resolution_list();
	
	// For each resolution included in list
	int c = 0;
	for(resolutionName = list[c]; resolutionName != NULL; resolutionName = list[c])
	{
		c++;
		gtk_list_store_append(resolutionStore, &iter);
		gtk_list_store_set(resolutionStore, &iter, 0 , resolutionName, -1);
	}
}

/**
 * Select active resolution
 */
void
select_active_resolution()
{
	GtkTreeModel* model;
	GtkTreeIter iter;
	gchar* resolution;
	gchar* tmp;

	// Select active resolution on server
	resolution = dbus_get_current_resolution();
	
	if( strcmp(resolution,"-1x-1") == 0 )
		return;
	
	tmp = resolution;
	model = gtk_combo_box_get_model(GTK_COMBO_BOX(resolutionComboBox));
	  
	// Find the currently set resolution
	gtk_tree_model_get_iter_first(model, &iter);
	do {
		gtk_tree_model_get(model, &iter, 0, &resolution , -1);
		if( g_strcasecmp( tmp , resolution ) == 0 )
		{
			// Set current iteration the active one
			gtk_combo_box_set_active_iter(GTK_COMBO_BOX(resolutionComboBox), &iter);
			return;
		}
	} while(gtk_tree_model_iter_next(model, &iter));

	// No index was found, select first one
	g_print("Warning : No active resolution found\n");
	gtk_combo_box_set_active(GTK_COMBO_BOX(resolutionComboBox), 0);
}

/**
 * Set the bitrate on the server with its value
 */
 
static void
select_bitrate(GtkComboBox* widget, gpointer data)
{
	GtkTreeModel* model;
	GtkTreeIter iter;
	int comboBoxIndex;
	gchar* name;
	
	comboBoxIndex = gtk_combo_box_get_active(widget);
	
	if(comboBoxIndex >= 0)
	{
		model = gtk_combo_box_get_model(widget);
		gtk_combo_box_get_active_iter(widget, &iter);
		gtk_tree_model_get(model, &iter, 0, &name, -1);	
		dbus_set_bitrate(name);
	}
}

/**
 * Fill bitrate store
 */
 
void
config_window_fill_bitrate_list()
{
	GtkTreeIter iter;
	gchar** list;
	gchar* bitrateName;
	gtk_list_store_clear(bitrateStore);
	gchar* temp = "kbps";
	
	// Call dbus to retrieve list
	list = dbus_get_bitrate_list();
	
	// For each bitrate included in list
	int c = 0;
	for(bitrateName = list[c]; bitrateName != NULL; bitrateName = list[c])
	{
		c++;
		gtk_list_store_append(bitrateStore, &iter);
		gtk_list_store_set(bitrateStore, &iter, 0 ,bitrateName , 1 , temp, -1);
	}
}

/**
 * Select active bitrate
 */
 
void
select_active_bitrate()
{
	GtkTreeModel* model;
	GtkTreeIter iter;
	gchar* bitrate;
	gchar* tmp;

	// Select active bitrate on server
	bitrate = dbus_get_current_bitrate();
	tmp = bitrate;
	model = gtk_combo_box_get_model(GTK_COMBO_BOX(bitrateComboBox));
	  
	// Find the currently set bitrate
	gtk_tree_model_get_iter_first(model, &iter);
	do {
		gtk_tree_model_get(model, &iter, 0, &bitrate , -1);
		if( g_strcasecmp( tmp , bitrate ) == 0 )
		{
			// Set current iteration the active one
			gtk_combo_box_set_active_iter(GTK_COMBO_BOX(bitrateComboBox), &iter);
			return;
		}
	} while(gtk_tree_model_iter_next(model, &iter));

	// No index was found, select first one
	g_print("Warning : No active bitrate found\n");
	gtk_combo_box_set_active(GTK_COMBO_BOX(bitrateComboBox), 0);
}

/**
 * Enables/disable local video capture in webcam settings tab
 */
static void
select_notebook_page(GtkNotebook* widget,  gpointer data)
{
	gint notebookPage;
	notebookPage = gtk_notebook_get_current_page(widget);
	printf("Notebook current page : %d\n", notebookPage);
	
	//Webcam Settings Page
	if(notebookPage == 4)
	{
		dbus_enable_local_video_pref();
		printf("Local video has been enabled in webcam settings \n");
	}
	else
	{
		dbus_disable_local_video_pref();
		printf("Local video has been disabled in webcam settings \n");
	}
}

/**
 * Refresh all audio settings
 */
static void
detect_all_audio_settings()
{
	// Update lists
	config_window_fill_output_audio_device_list();
	config_window_fill_input_audio_device_list();
	
	// Select active device in combo box
	//select_active_output_audio_device();
	//select_active_input_audio_device();
}

/**
 * Refresh all webcam devices
 */
static void
detect_webcam_device()
{
	// Update lists
	config_window_fill_webcam_device_list();
	
}

/**
 * Delete an account
 */
static void
delete_account(GtkWidget *widget, gpointer data)
{
	if(selectedAccount)
	{
		dbus_remove_account(selectedAccount->accountID);
	}
}

/**
 * Edit an account
 */
static void
edit_account(GtkWidget *widget, gpointer data)
{
	if(selectedAccount)
	{
		show_account_window(selectedAccount);
	}
}

/**
 * Add an account
 */
static void
add_account(GtkWidget *widget, gpointer data)
{
	show_account_window(NULL);
}

int 
is_ringtone_enabled( void )
{
  return dbus_is_ringtone_enabled();  
}

void
start_hidden( void )
{
  dbus_start_hidden();
}

void
set_popup_mode( void )
{
  dbus_switch_popup_mode();
}

void
set_notif_level(  )
{
  dbus_set_notify();
}

void
set_mail_notif( )
{
  dbus_set_mail_notify( );
}

void
update_max_value( GtkRange* scale )
{
  dbus_set_max_calls(gtk_range_get_value( GTK_RANGE( scale )));
}

void
clean_history( void )
{
  call_list_clean_history();
}

void 
ringtone_enabled( void )
{
  dbus_ringtone_enabled();  
}

void
ringtone_changed( GtkFileChooser *chooser , GtkLabel *label)
{
  gchar* tone = gtk_file_chooser_get_filename( GTK_FILE_CHOOSER( chooser ));
  dbus_set_ringtone_choice( tone );
}

gchar*
get_ringtone_choice( void )
{
  return dbus_get_ringtone_choice();
}

/**
 * Call back when the user click on an account in the list
 */
static void
select_account(GtkTreeSelection *selection, GtkTreeModel *model)
{
	GtkTreeIter iter;
	GValue val;

	if (!gtk_tree_selection_get_selected(selection, &model, &iter))
	{
		selectedAccount = NULL;
		gtk_widget_set_sensitive(GTK_WIDGET(accountMoveUpButton), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(accountMoveDownButton), FALSE);
		return;
	}

	val.g_type = G_TYPE_POINTER;
	gtk_tree_model_get_value(model, &iter, COLUMN_ACCOUNT_DATA, &val);

	selectedAccount = (account_t*)g_value_get_pointer(&val);
	g_value_unset(&val);

	if(selectedAccount)
	{
		gtk_widget_set_sensitive(GTK_WIDGET(editButton), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(deleteButton), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(accountMoveUpButton), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(accountMoveDownButton), TRUE);
	}
	g_print("select");
}

/**
 * Toggle move buttons on if a codec is selected, off elsewise
 * For the Audio Settings tab
 */
static void
select_codec(GtkTreeSelection *selection, GtkTreeModel *model)
{
	GtkTreeIter iter;
	
	if(!gtk_tree_selection_get_selected(selection, &model, &iter))
	{
		gtk_widget_set_sensitive(GTK_WIDGET(codecMoveUpButton), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(codecMoveDownButton), FALSE);
	}
	else
	{
		gtk_widget_set_sensitive(GTK_WIDGET(codecMoveUpButton), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(codecMoveDownButton), TRUE);
	}
}

/**
 * Toggle move buttons on if a video codec is selected, off elsewise
 * For the Video Settings tab
 */
static void
select_codec_video(GtkTreeSelection *selection, GtkTreeModel *model)
{
	GtkTreeIter iter;
	
	if(!gtk_tree_selection_get_selected(selection, &model, &iter))
	{
		gtk_widget_set_sensitive(GTK_WIDGET(moveUpButtonVideo), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(moveDownButtonVideo), FALSE);
	}
	else
	{
		gtk_widget_set_sensitive(GTK_WIDGET(moveUpButtonVideo), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(moveDownButtonVideo), TRUE);
	}
}


/**
 * Toggle active value of codec on click and update changes to the deamon
 * and in configuration files
 * For the Audio Settings Tab
 */
static void
codec_active_toggled(GtkCellRendererToggle *renderer, gchar *path, gpointer data)
{
	GtkTreeIter iter;
	GtkTreePath *treePath;
	GtkTreeModel *model;
	gboolean active;
	char* name;
	
	// Get path of clicked codec active toggle box
	treePath = gtk_tree_path_new_from_string(path);
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(data));
	gtk_tree_model_get_iter(model, &iter, treePath);

	// Get active value and name at iteration
	gtk_tree_model_get(model, &iter,
			COLUMN_CODEC_ACTIVE, &active,
			COLUMN_CODEC_NAME, &name,
			-1);
	
	printf("%s\n", name);

	// Toggle active value
	active = !active;
	
	// Store value
	gtk_list_store_set(GTK_LIST_STORE(model), &iter,
			COLUMN_CODEC_ACTIVE, active,
			-1);

	gtk_tree_path_free(treePath);

	// Modify codec queue to represent change	
	if(active)
		codec_set_active(name);
	else
		codec_set_inactive(name);
	
	// Perpetuate changes to the deamon
	codec_list_update_to_daemon();
}

static void
enable_account(GtkCellRendererToggle *rend , gchar* path,  gpointer data )
{
  GtkTreeIter iter;
  GtkTreePath *treePath;
  GtkTreeModel *model;
  gboolean enable;
  account_t* acc ;

  // Get path of clicked codec active toggle box
  treePath = gtk_tree_path_new_from_string(path);
  model = gtk_tree_view_get_model(GTK_TREE_VIEW(data));
  gtk_tree_model_get_iter(model, &iter, treePath);

  // Get pointer on object
  gtk_tree_model_get(model, &iter,
                      COLUMN_ACCOUNT_ACTIVE, &enable,
                      COLUMN_ACCOUNT_DATA, &acc,
                      -1);
  enable = !enable;

  // Store value
  gtk_list_store_set(GTK_LIST_STORE(model), &iter,
                     COLUMN_ACCOUNT_ACTIVE, enable,
                    -1);

  gtk_tree_path_free(treePath);

  // Modify account state       
  g_hash_table_replace( acc->properties , g_strdup(ACCOUNT_ENABLED) , g_strdup((enable == 1)? "TRUE":"FALSE"));
  dbus_send_register( acc->accountID , enable );
}

/**
 * Toggle active value of video codec on click and update changes to the deamon
 * and in configuration files
 * For the Video Settings tab
 */
static void
video_codec_active_toggled(GtkCellRendererToggle *renderer, gchar *path, gpointer data)
{
	GtkTreeIter iter;
	GtkTreePath *treePath;
	GtkTreeModel *model;
	gboolean active;
	char* name;
	
	// Get path of clicked codec active toggle box
	treePath = gtk_tree_path_new_from_string(path);
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(data));
	gtk_tree_model_get_iter(model, &iter, treePath);

	// Get active value and name at iteration
	gtk_tree_model_get(model, &iter,
			COLUMN_CODEC_ACTIVE, &active,
			COLUMN_CODEC_NAME, &name,
			-1);
	
	printf("%s\n", name);

	// Toggle active value
	active = !active;
	
	// Store value
	gtk_list_store_set(GTK_LIST_STORE(model), &iter,
			COLUMN_CODEC_ACTIVE, active,
			-1);

	gtk_tree_path_free(treePath);

	// Modify codec queue to represent change	
	if(active)
		video_codec_set_active(name);
	else
		video_codec_set_inactive(name);
	
	// Perpetuate changes to the deamon
	video_codec_list_update_to_daemon();
}

/**
 * Move codec in list depending on direction and selected codec and
 * update changes in the deamon list and the configuration files
 * For the Audio Settings tab
 */
static void
codec_move(gboolean moveUp, gpointer data)
{
	GtkTreeIter iter;
	GtkTreeIter *iter2;
	GtkTreeView *treeView;
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	GtkTreePath *treePath;
	gchar *path;
	
	// Get view, model and selection of codec store
	treeView = GTK_TREE_VIEW(data);
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeView));
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeView));
	
	// Find selected iteration and create a copy
	gtk_tree_selection_get_selected(GTK_TREE_SELECTION(selection), &model, &iter);
	iter2 = gtk_tree_iter_copy(&iter);
	
	// Find path of iteration
	path = gtk_tree_model_get_string_from_iter(GTK_TREE_MODEL(model), &iter);
	treePath = gtk_tree_path_new_from_string(path);
	gint *indices = gtk_tree_path_get_indices(treePath);
	gint indice = indices[0];
	
	// Depending on button direction get new path
	if(moveUp)
		gtk_tree_path_prev(treePath);
	else
		gtk_tree_path_next(treePath);
	gtk_tree_model_get_iter(model, &iter, treePath);
	
	// Swap iterations if valid
	if(gtk_list_store_iter_is_valid(GTK_LIST_STORE(model), &iter))
		gtk_list_store_swap(GTK_LIST_STORE(model), &iter, iter2);
	
	// Scroll to new position
	gtk_tree_view_scroll_to_cell(treeView, treePath, NULL, FALSE, 0, 0);
	
	// Free resources
	gtk_tree_path_free(treePath);
	gtk_tree_iter_free(iter2);
	g_free(path);
	
	// Perpetuate changes in codec queue
	if(moveUp)
		codec_list_move_codec_up(indice);
	else
		codec_list_move_codec_down(indice);
	
	// Perpetuate changes to the deamon
	codec_list_update_to_daemon();
}

static void
account_move(gboolean moveUp, gpointer data)
{
	GtkTreeIter iter;
	GtkTreeIter *iter2;
	GtkTreeView *treeView;
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	GtkTreePath *treePath;
	gchar *path;
	
	// Get view, model and selection of codec store
	treeView = GTK_TREE_VIEW(data);
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeView));
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeView));
    
	// Find selected iteration and create a copy
	gtk_tree_selection_get_selected(GTK_TREE_SELECTION(selection), &model, &iter);
	iter2 = gtk_tree_iter_copy(&iter);
	
	// Find path of iteration
	path = gtk_tree_model_get_string_from_iter(GTK_TREE_MODEL(model), &iter);
	treePath = gtk_tree_path_new_from_string(path);
	gint *indices = gtk_tree_path_get_indices(treePath);
	gint indice = indices[0];
	
	// Depending on button direction get new path
	if(moveUp)
		gtk_tree_path_prev(treePath);
	else
		gtk_tree_path_next(treePath);
	gtk_tree_model_get_iter(model, &iter, treePath);
	
	// Swap iterations if valid
	if(gtk_list_store_iter_is_valid(GTK_LIST_STORE(model), &iter))
		gtk_list_store_swap(GTK_LIST_STORE(model), &iter, iter2);
	
	// Scroll to new position
	gtk_tree_view_scroll_to_cell(treeView, treePath, NULL, FALSE, 0, 0);
	
	// Free resources
	gtk_tree_path_free(treePath);
	gtk_tree_iter_free(iter2);
	g_free(path);
	
	// Perpetuate changes in account queue
	if(moveUp)
		account_list_move_up(indice);
	else
		account_list_move_down(indice);
}

/**
 * Move codec in list depending on direction and selected video codec and
 * update changes in the deamon list and the configuration files
 * For the Video Settings tab
 */
static void
video_codec_move(gboolean moveUp, gpointer data)
{
	GtkTreeIter iter;
	GtkTreeIter *iter2;
	GtkTreeView *treeView;
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	GtkTreePath *treePath;
	gchar *path;
	
	// Get view, model and selection of codec store
	treeView = GTK_TREE_VIEW(data);
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeView));
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeView));
	
	// Find selected iteration and create a copy
	gtk_tree_selection_get_selected(GTK_TREE_SELECTION(selection), &model, &iter);
	iter2 = gtk_tree_iter_copy(&iter);
	
	// Find path of iteration
	path = gtk_tree_model_get_string_from_iter(GTK_TREE_MODEL(model), &iter);
	treePath = gtk_tree_path_new_from_string(path);
	gint *indices = gtk_tree_path_get_indices(treePath);
	gint indice = indices[0];
	
	// Depending on button direction get new path
	if(moveUp)
		gtk_tree_path_prev(treePath);
	else
		gtk_tree_path_next(treePath);
	gtk_tree_model_get_iter(model, &iter, treePath);
	
	// Swap iterations if valid
	if(gtk_list_store_iter_is_valid(GTK_LIST_STORE(model), &iter))
		gtk_list_store_swap(GTK_LIST_STORE(model), &iter, iter2);
	
	// Scroll to new position
	gtk_tree_view_scroll_to_cell(treeView, treePath, NULL, FALSE, 0, 0);
	
	// Free resources
	gtk_tree_path_free(treePath);
	gtk_tree_iter_free(iter2);
	g_free(path);
	
	// Perpetuate changes in codec queue
	if(moveUp)
		video_codec_list_move_codec_up(indice);
	else
		video_codec_list_move_codec_down(indice);
	
	// Perpetuate changes to the deamon
	video_codec_list_update_to_daemon();
}

/**
 * Called from move up codec button signal
 */
static void
codec_move_up(GtkButton *button, gpointer data)
{
  // Change tree view ordering and get indice changed
  codec_move(TRUE, data);
}

/**
 * Called from move down codec button signal
 */
static void
codec_move_down(GtkButton *button, gpointer data)
{
  // Change tree view ordering and get indice changed
  codec_move(FALSE, data);
}

/**
 * Called from move up video codec button signal
 */
static void
video_codec_move_up(GtkButton *button, gpointer data)
{
	// Change tree view ordering and get indice changed
	video_codec_move(TRUE, data);
}

/**
 * Called from move down video codec button signal
 */
static void
video_codec_move_down(GtkButton *button, gpointer data)
{
	// Change tree view ordering and get indice changed
	video_codec_move(FALSE, data);
}

/**
 * Called from move up account button signal
 */
static void
account_move_up(GtkButton *button, gpointer data)
{
  // Change tree view ordering and get indice changed
  account_move(TRUE, data);
}

/**
 * Called from move down account button signal
 */
static void
account_move_down(GtkButton *button, gpointer data)
{
  // Change tree view ordering and get indice changed
  account_move(FALSE, data);
}

/**
 * Set the brightness on the server with its value
 */
static void
set_brightness(GtkScale* scale, gpointer data)
{
	gdouble value;
	value = gtk_range_get_value(GTK_RANGE(scale));
	printf("set brightness to: %f \n", value);
	dbus_set_brightness(value);
	
}

/**
 * Set the contrast on the server with its value
 */
static void
set_contrast(GtkScale* scale, gpointer data)
{
	gdouble value;
	value = gtk_range_get_value(GTK_RANGE(scale));
	printf("set contrast to: %f \n", value);
	dbus_set_contrast(value);
	
}

/**
 * Set the colour on the server with its value
 */
static void
set_colour(GtkScale* scale, gpointer data)
{
	gdouble value;
	value = gtk_range_get_value(GTK_RANGE(scale));
	dbus_set_colour(value);
	
}

static void activate_checkbox(GtkToggleButton *togglebutton, gpointer user_data)
{
	gboolean status = gtk_toggle_button_get_active(togglebutton);
	set_enable_webcam_checkbox_status(status);
}

static void cancel_checkbox(GtkToggleButton *togglebutton, gpointer user_data)
{
	gboolean status = gtk_toggle_button_get_active(togglebutton);
	set_disable_webcam_checkbox_status(status);
}

gboolean get_enable_webcam_checkbox_status()
{
	enableStatus = dbus_get_enable_checkbox_status();
	return enableStatus;
}

gboolean get_disable_webcam_checkbox_status()
{
	disableStatus = dbus_get_disable_checkbox_status();
	return disableStatus;
}
void set_enable_webcam_checkbox_status(gboolean status)
{
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(activateCheckBox), status);
	enableStatus = status;
	dbus_set_enable_checkbox_status(status);
}

void set_disable_webcam_checkbox_status(gboolean status)
{
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cancelCheckBox), status);
	disableStatus = status;
	dbus_set_disable_checkbox_status(status);
}

void video_settings_checkbox_init()
{
	enableStatus = dbus_get_enable_checkbox_status();
	disableStatus = dbus_get_disable_checkbox_status();
}
/**
 * Create table widget for video codecs
 */
GtkWidget*
create_video_codec_table()
{
	
	// \todo populate the list with video codec instead of audio codec
	
	GtkWidget *ret;
	GtkWidget *scrolledWindow;
	GtkWidget *buttonBox;
	
	GtkListStore *codecStore;
	GtkCellRenderer *renderer;
	GtkTreeSelection *treeSelection;
	GtkTreeViewColumn *treeViewColumn;
	
	ret = gtk_hbox_new(FALSE, 10);
	gtk_container_set_border_width(GTK_CONTAINER(ret), 10);
	
	scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledWindow), GTK_SHADOW_IN);
	
	gtk_box_pack_start(GTK_BOX(ret), scrolledWindow, TRUE, TRUE, 0);
	codecStore = gtk_list_store_new(2,
			G_TYPE_BOOLEAN,		// Active
			G_TYPE_STRING		// Name
			);
	
	// Create codec tree view with list store
	videoCodecTreeView = gtk_tree_view_new_with_model(GTK_TREE_MODEL(codecStore));
	
	// Get tree selection manager
	treeSelection = gtk_tree_view_get_selection(GTK_TREE_VIEW(videoCodecTreeView));
	g_signal_connect(G_OBJECT(treeSelection), "changed",
			G_CALLBACK (select_codec_video),
			codecStore);
	
	// Active column
	renderer = gtk_cell_renderer_toggle_new();
	treeViewColumn = gtk_tree_view_column_new_with_attributes("", renderer, "active", COLUMN_CODEC_ACTIVE, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(videoCodecTreeView), treeViewColumn);

	// Toggle codec active property on clicked
	g_signal_connect(G_OBJECT(renderer), "toggled", G_CALLBACK(video_codec_active_toggled), (gpointer)videoCodecTreeView);
	
	// Name column
	renderer = gtk_cell_renderer_text_new();
	treeViewColumn = gtk_tree_view_column_new_with_attributes("Name", renderer, "markup", COLUMN_CODEC_NAME, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(videoCodecTreeView), treeViewColumn);
	
	g_object_unref(G_OBJECT(codecStore));
	gtk_container_add(GTK_CONTAINER(scrolledWindow), videoCodecTreeView);
	
	// Create button box
	buttonBox = gtk_vbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(buttonBox), 10);
	gtk_box_pack_start(GTK_BOX(ret), buttonBox, FALSE, FALSE, 0);
	
	moveUpButtonVideo = gtk_button_new_from_stock(GTK_STOCK_GO_UP);
	gtk_widget_set_sensitive(GTK_WIDGET(moveUpButtonVideo), FALSE);
	gtk_box_pack_start(GTK_BOX(buttonBox), moveUpButtonVideo, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(moveUpButtonVideo), "clicked", G_CALLBACK(video_codec_move_up), videoCodecTreeView);
	
	moveDownButtonVideo = gtk_button_new_from_stock(GTK_STOCK_GO_DOWN);
	gtk_widget_set_sensitive(GTK_WIDGET(moveDownButtonVideo), FALSE);
	gtk_box_pack_start(GTK_BOX(buttonBox), moveDownButtonVideo, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(moveDownButtonVideo), "clicked", G_CALLBACK(video_codec_move_down), videoCodecTreeView);
	
	config_window_fill_video_codec_list();
	
	return ret;
}

/**
 * Create table widget for audio codecs
 */
GtkWidget*
create_codec_table()
{
	GtkWidget *ret;
	GtkWidget *scrolledWindow;
	GtkWidget *buttonBox;
	
	GtkListStore *codecStore;
	GtkCellRenderer *renderer;
	GtkTreeSelection *treeSelection;
	GtkTreeViewColumn *treeViewColumn;
	
	ret = gtk_hbox_new(FALSE, 10);
	gtk_container_set_border_width(GTK_CONTAINER(ret), 10);
	
	scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledWindow), GTK_SHADOW_IN);
	gtk_box_pack_start(GTK_BOX(ret), scrolledWindow, TRUE, TRUE, 0);
	
	codecStore = gtk_list_store_new(CODEC_COLUMN_COUNT,
			G_TYPE_BOOLEAN,		// Active
			G_TYPE_STRING,		// Name
			G_TYPE_STRING,		// Frequency
			G_TYPE_STRING,		// Bit rate
			G_TYPE_STRING		// Bandwith
			);
	
	// Create codec tree view with list store
	codecTreeView = gtk_tree_view_new_with_model(GTK_TREE_MODEL(codecStore));
	
	// Get tree selection manager
	treeSelection = gtk_tree_view_get_selection(GTK_TREE_VIEW(codecTreeView));
	g_signal_connect(G_OBJECT(treeSelection), "changed",
			G_CALLBACK (select_codec),
			codecStore);
	
	// Active column
	renderer = gtk_cell_renderer_toggle_new();
	treeViewColumn = gtk_tree_view_column_new_with_attributes("", renderer, "active", COLUMN_CODEC_ACTIVE, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(codecTreeView), treeViewColumn);

	// Toggle codec active property on clicked
	g_signal_connect(G_OBJECT(renderer), "toggled", G_CALLBACK(codec_active_toggled), (gpointer)codecTreeView);
	
	// Name column
	renderer = gtk_cell_renderer_text_new();
	treeViewColumn = gtk_tree_view_column_new_with_attributes(_("Name"), renderer, "markup", COLUMN_CODEC_NAME, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(codecTreeView), treeViewColumn);
	
	// Bit rate column
	renderer = gtk_cell_renderer_text_new();
	treeViewColumn = gtk_tree_view_column_new_with_attributes(_("Frequency"), renderer, "text", COLUMN_CODEC_FREQUENCY, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(codecTreeView), treeViewColumn);
	
	// Bandwith column
	renderer = gtk_cell_renderer_text_new();
	treeViewColumn = gtk_tree_view_column_new_with_attributes(_("Bitrate"), renderer, "text", COLUMN_CODEC_BITRATE, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(codecTreeView), treeViewColumn);
	
	// Frequency column
	renderer = gtk_cell_renderer_text_new();
	treeViewColumn = gtk_tree_view_column_new_with_attributes(_("Bandwidth"), renderer, "text", COLUMN_CODEC_BANDWIDTH, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(codecTreeView), treeViewColumn);
	
	g_object_unref(G_OBJECT(codecStore));
	gtk_container_add(GTK_CONTAINER(scrolledWindow), codecTreeView);
	
	// Create button box
	buttonBox = gtk_vbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(buttonBox), 10);
	gtk_box_pack_start(GTK_BOX(ret), buttonBox, FALSE, FALSE, 0);
	
	codecMoveUpButton = gtk_button_new_from_stock(GTK_STOCK_GO_UP);
	gtk_widget_set_sensitive(GTK_WIDGET(codecMoveUpButton), FALSE);
	gtk_box_pack_start(GTK_BOX(buttonBox), codecMoveUpButton, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(codecMoveUpButton), "clicked", G_CALLBACK(codec_move_up), codecTreeView);
	
	codecMoveDownButton = gtk_button_new_from_stock(GTK_STOCK_GO_DOWN);
	gtk_widget_set_sensitive(GTK_WIDGET(codecMoveDownButton), FALSE);
	gtk_box_pack_start(GTK_BOX(buttonBox), codecMoveDownButton, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(codecMoveDownButton), "clicked", G_CALLBACK(codec_move_down), codecTreeView);
	
	config_window_fill_codec_list();

	return ret;
}

/**
 * Account settings tab
 */
GtkWidget *
create_accounts_tab()
{
	GtkWidget *ret;
	GtkWidget *scrolledWindow;
	GtkWidget *treeView;
	GtkWidget *buttonBox;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *treeViewColumn;
	GtkTreeSelection *treeSelection;

	selectedAccount = NULL;

	ret = gtk_vbox_new(FALSE, 10); 
	gtk_container_set_border_width(GTK_CONTAINER (ret), 10);

	scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledWindow), GTK_SHADOW_IN);
	gtk_box_pack_start(GTK_BOX(ret), scrolledWindow, TRUE, TRUE, 0);

	accountStore = gtk_list_store_new(COLUMN_ACCOUNT_COUNT,
			G_TYPE_STRING,  // Name
			G_TYPE_STRING,  // Protocol
			G_TYPE_STRING,  // Status
			G_TYPE_BOOLEAN, // Enabled / Disabled
			G_TYPE_POINTER  // Pointer to the Object
			);

	treeView = gtk_tree_view_new_with_model(GTK_TREE_MODEL(accountStore));
	treeSelection = gtk_tree_view_get_selection(GTK_TREE_VIEW (treeView));
	g_signal_connect(G_OBJECT (treeSelection), "changed",
			G_CALLBACK (select_account),
			accountStore);

	renderer = gtk_cell_renderer_text_new();
	treeViewColumn = gtk_tree_view_column_new_with_attributes ("Alias",
			renderer,
			"markup", COLUMN_ACCOUNT_ALIAS,
			NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW(treeView), treeViewColumn);
	
	// A double click on the account line opens the window to edit the account
	g_signal_connect( G_OBJECT( treeView ) , "row-activated" , G_CALLBACK( edit_account ) , NULL );

	renderer = gtk_cell_renderer_text_new();
	treeViewColumn = gtk_tree_view_column_new_with_attributes (_("Protocol"),
			renderer,
			"markup", COLUMN_ACCOUNT_TYPE,
			NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW(treeView), treeViewColumn);

	renderer = gtk_cell_renderer_text_new();
	treeViewColumn = gtk_tree_view_column_new_with_attributes (_("Status"),
			renderer,
			"markup", COLUMN_ACCOUNT_STATUS,
			NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW(treeView), treeViewColumn);
	
	renderer = gtk_cell_renderer_toggle_new();
	treeViewColumn = gtk_tree_view_column_new_with_attributes("", renderer, "active", COLUMN_ACCOUNT_ACTIVE , NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(treeView), treeViewColumn);
	g_signal_connect( G_OBJECT(renderer) , "toggled" , G_CALLBACK(enable_account), (gpointer)treeView );

	g_object_unref(G_OBJECT(accountStore));
	gtk_container_add(GTK_CONTAINER(scrolledWindow), treeView);

	 // Create button box
        buttonBox = gtk_vbox_new(FALSE, 0);
        gtk_container_set_border_width(GTK_CONTAINER(buttonBox), 10);
        gtk_box_pack_start(GTK_BOX(ret), buttonBox, FALSE, FALSE, 0);

        accountMoveUpButton = gtk_button_new_from_stock(GTK_STOCK_GO_UP);
        gtk_widget_set_sensitive(GTK_WIDGET(accountMoveUpButton), FALSE);
        gtk_box_pack_start(GTK_BOX(buttonBox), accountMoveUpButton, FALSE, FALSE, 0);
        g_signal_connect(G_OBJECT(accountMoveUpButton), "clicked", G_CALLBACK(account_move_up), treeView);

        accountMoveDownButton = gtk_button_new_from_stock(GTK_STOCK_GO_DOWN);
        gtk_widget_set_sensitive(GTK_WIDGET(accountMoveDownButton), FALSE);
        gtk_box_pack_start(GTK_BOX(buttonBox), accountMoveDownButton, FALSE, FALSE, 0);
        g_signal_connect(G_OBJECT(accountMoveDownButton), "clicked", G_CALLBACK(account_move_down), treeView);
	
	/* The buttons to press! */
	buttonBox = gtk_hbutton_box_new();
	gtk_box_set_spacing(GTK_BOX(buttonBox), 10); //GAIM_HIG_BOX_SPACE
	gtk_button_box_set_layout(GTK_BUTTON_BOX(buttonBox), GTK_BUTTONBOX_CENTER);
	gtk_box_pack_start(GTK_BOX(ret), buttonBox, FALSE, FALSE, 0);
	gtk_widget_show (buttonBox); 

	addButton = gtk_button_new_from_stock (GTK_STOCK_ADD);
	g_signal_connect_swapped(G_OBJECT(addButton), "clicked",
			G_CALLBACK(add_account), NULL);
	gtk_box_pack_start(GTK_BOX(buttonBox), addButton, FALSE, FALSE, 0);
	gtk_widget_show(addButton);

	editButton = gtk_button_new_from_stock (GTK_STOCK_EDIT);
	g_signal_connect_swapped(G_OBJECT(editButton), "clicked",
			G_CALLBACK(edit_account), NULL);
	gtk_box_pack_start(GTK_BOX(buttonBox), editButton, FALSE, FALSE, 0);
	gtk_widget_show(editButton);
	
	deleteButton = gtk_button_new_from_stock (GTK_STOCK_REMOVE);
	g_signal_connect_swapped(G_OBJECT(deleteButton), "clicked",
			G_CALLBACK(delete_account), NULL);
	gtk_box_pack_start(GTK_BOX(buttonBox), deleteButton, FALSE, FALSE, 0);
	gtk_widget_show(deleteButton);
	
	gtk_widget_show_all(ret);

	config_window_fill_account_list();

	return ret;
}

/**
 * Audio settings tab
 */
GtkWidget*
create_audio_tab ()
{
	GtkWidget *ret;
	
	GtkWidget *deviceFrame;
	GtkWidget *deviceBox;
	GtkWidget *deviceTable;
	GtkWidget *codecFrame;
	GtkWidget *codecBox;
	GtkWidget *enableTone;
	GtkWidget *fileChooser;
	
	GtkWidget *titleLabel;
	GtkWidget *refreshButton;
	GtkCellRenderer *renderer;
	
	GtkWidget *codecTable;
	
	// Main widget
	ret = gtk_vbox_new(FALSE, 10);
    gtk_container_set_border_width(GTK_CONTAINER(ret), 10);
    
    // Device section label
    deviceFrame = gtk_frame_new(_("Devices"));
    gtk_box_pack_start(GTK_BOX(ret), deviceFrame, FALSE, FALSE, 0);
    gtk_widget_show( deviceFrame );

	
    // Main device widget
    deviceBox = gtk_hbox_new(FALSE, 10);
    gtk_box_pack_start(GTK_BOX(deviceFrame), deviceBox, FALSE, FALSE, 0);
    gtk_widget_show( deviceBox );

    gtk_container_add( GTK_CONTAINER(deviceFrame) , deviceBox);

    // Main device widget
	deviceTable = gtk_table_new(4, 3, FALSE);
	gtk_table_set_col_spacing(GTK_TABLE(deviceTable), 0, 40);
	gtk_box_set_spacing(GTK_BOX(deviceTable), 0);
	gtk_box_pack_start(GTK_BOX(deviceBox), deviceTable, TRUE, TRUE, 0);
	gtk_widget_show(deviceTable);
	
	// Device : Audio manager
	// Create title label
	/*
	titleLabel = gtk_label_new("Alsa plug-IN:");
	gtk_misc_set_alignment(GTK_MISC(titleLabel), 0, 0.5);
	gtk_table_attach(GTK_TABLE(deviceTable), titleLabel, 0, 1, 0, 1, GTK_FILL | GTK_EXPAND, GTK_SHRINK, 0, 0);
	gtk_widget_show(titleLabel);
	// Set choices of audio managers
	inputAudioPluginStore = gtk_list_store_new(1, G_TYPE_STRING);
	config_window_fill_input_audio_plugin_list();
	comboBox = gtk_combo_box_new_with_model(GTK_TREE_MODEL(inputAudioPluginStore));
	gtk_combo_box_set_active(GTK_COMBO_BOX(comboBox), 0);
	gtk_label_set_mnemonic_widget(GTK_LABEL(titleLabel), comboBox);
	g_signal_connect(G_OBJECT(comboBox), "changed", G_CALLBACK(select_input_audio_plugin), comboBox);
	
  	// Set rendering
	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(comboBox), renderer, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(comboBox), renderer, "text", 0, NULL);
	gtk_table_attach(GTK_TABLE(deviceTable), comboBox, 1, 2, 0, 1, GTK_FILL | GTK_EXPAND, GTK_SHRINK, 0, 0);
	gtk_widget_show(comboBox);
	*/
	// Create title label
	titleLabel = gtk_label_new(_("ALSA plugin"));
	gtk_misc_set_alignment(GTK_MISC(titleLabel), 0, 0.5);
	gtk_table_attach(GTK_TABLE(deviceTable), titleLabel, 1, 2, 0, 1, GTK_FILL | GTK_EXPAND, GTK_SHRINK, 0, 0);
	gtk_widget_show(titleLabel);
	// Set choices of audio managers
	outputAudioPluginStore = gtk_list_store_new(1, G_TYPE_STRING);
	config_window_fill_output_audio_plugin_list();
	pluginComboBox = gtk_combo_box_new_with_model(GTK_TREE_MODEL(outputAudioPluginStore));
	select_active_output_audio_plugin();
	gtk_label_set_mnemonic_widget(GTK_LABEL(titleLabel), pluginComboBox);
	g_signal_connect(G_OBJECT(pluginComboBox), "changed", G_CALLBACK(select_output_audio_plugin), pluginComboBox);
	
  	// Set rendering
	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(pluginComboBox), renderer, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(pluginComboBox), renderer, "text", 0, NULL);
	gtk_table_attach(GTK_TABLE(deviceTable), pluginComboBox, 2, 3, 0, 1, GTK_FILL | GTK_EXPAND, GTK_SHRINK, 0, 0);
	gtk_widget_show(pluginComboBox);
	
	// Device : Output device
	// Create title label
	titleLabel = gtk_label_new(_("Output peripheral"));
    gtk_misc_set_alignment(GTK_MISC(titleLabel), 0, 0.5);
    gtk_table_attach(GTK_TABLE(deviceTable), titleLabel, 1, 2, 1, 2, GTK_FILL | GTK_EXPAND, GTK_SHRINK, 0, 0);
    gtk_widget_show(titleLabel);
	// Set choices of output devices
	outputAudioDeviceManagerStore = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_INT);
	config_window_fill_output_audio_device_list();
	outputDeviceComboBox = gtk_combo_box_new_with_model(GTK_TREE_MODEL(outputAudioDeviceManagerStore));
	select_active_output_audio_device();
  	gtk_label_set_mnemonic_widget(GTK_LABEL(titleLabel), outputDeviceComboBox);
	g_signal_connect(G_OBJECT(outputDeviceComboBox), "changed", G_CALLBACK(select_audio_output_device), outputDeviceComboBox);

	// Set rendering
	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(outputDeviceComboBox), renderer, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(outputDeviceComboBox), renderer, "text", 0, NULL);
	gtk_table_attach(GTK_TABLE(deviceTable), outputDeviceComboBox, 2, 3, 1, 2, GTK_FILL | GTK_EXPAND, GTK_SHRINK, 0, 0);
	gtk_widget_show(outputDeviceComboBox);
	
	// Device : Input device
	// Create title label
	titleLabel = gtk_label_new(_("Input peripheral"));
    gtk_misc_set_alignment(GTK_MISC(titleLabel), 0, 0.5);
    gtk_table_attach(GTK_TABLE(deviceTable), titleLabel, 1, 2, 2, 3, GTK_FILL | GTK_EXPAND, GTK_SHRINK, 0, 0);
	gtk_widget_show(titleLabel);
	// Set choices of output devices
	inputAudioDeviceManagerStore = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_INT);
	config_window_fill_input_audio_device_list();
	inputDeviceComboBox = gtk_combo_box_new_with_model(GTK_TREE_MODEL(inputAudioDeviceManagerStore));
	select_active_input_audio_device();
	gtk_label_set_mnemonic_widget(GTK_LABEL(titleLabel), inputDeviceComboBox);
	g_signal_connect(G_OBJECT(inputDeviceComboBox), "changed", G_CALLBACK(select_audio_input_device), inputDeviceComboBox);

	// Set rendering
	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(inputDeviceComboBox), renderer, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(inputDeviceComboBox), renderer, "text", 0, NULL);
    gtk_table_attach(GTK_TABLE(deviceTable), inputDeviceComboBox, 2, 3, 2, 3, GTK_FILL | GTK_EXPAND, GTK_SHRINK, 0, 0);
	gtk_widget_show(inputDeviceComboBox);
	
	// Create detect button
	refreshButton = gtk_button_new_with_label(_("Detect all"));
	gtk_button_set_image(GTK_BUTTON(refreshButton), gtk_image_new_from_stock(GTK_STOCK_REFRESH, GTK_ICON_SIZE_BUTTON));
	gtk_table_attach(GTK_TABLE(deviceTable), refreshButton, 2, 3, 3, 4, GTK_EXPAND, GTK_EXPAND, 0, 0);
	// Set event on selection
	g_signal_connect(G_OBJECT(refreshButton), "clicked", G_CALLBACK(detect_all_audio_settings), NULL);
	
	//select_active_output_audio_plugin();
    // Codec section label
    codecFrame = gtk_frame_new(_("Codecs"));
    gtk_misc_set_alignment(GTK_MISC(codecFrame), 0, 0.5);
    gtk_box_pack_start(GTK_BOX(ret), codecFrame, FALSE, FALSE, 0);
    gtk_widget_show(codecFrame);

    // Main codec widget
	codecBox = gtk_hbox_new(FALSE, 10);
	gtk_box_pack_start(GTK_BOX(codecFrame), codecBox, FALSE, FALSE, 0);
	gtk_widget_show(codecBox);
	
	gtk_container_add( GTK_CONTAINER( codecFrame ) , codecBox );
	// Codec : List
	codecTable = create_codec_table();
	gtk_widget_set_size_request(GTK_WIDGET(codecTable), -1, 150);
	gtk_box_pack_start(GTK_BOX(codecBox), codecTable, TRUE, TRUE, 0);
	gtk_widget_show(codecTable);

    // check button to enable ringtones
	GtkWidget* box = gtk_hbox_new( TRUE , 1);
	gtk_box_pack_start( GTK_BOX(ret) , box , FALSE , FALSE , 1);
	enableTone = gtk_check_button_new_with_mnemonic( _("_Enable ringtones"));
      gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(enableTone), dbus_is_ringtone_enabled() );
      gtk_box_pack_start( GTK_BOX(box) , enableTone , TRUE , TRUE , 1);
      g_signal_connect(G_OBJECT( enableTone) , "clicked" , G_CALLBACK( ringtone_enabled ) , NULL);
    // file chooser button
	fileChooser = gtk_file_chooser_button_new(_("Choose a ringtone"), GTK_FILE_CHOOSER_ACTION_OPEN);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER( fileChooser) , g_get_home_dir());	
	gtk_file_chooser_set_filename(GTK_FILE_CHOOSER( fileChooser) , get_ringtone_choice());	
	g_signal_connect( G_OBJECT( fileChooser ) , "selection_changed" , G_CALLBACK( ringtone_changed ) , NULL );
	GtkFileFilter *filter = gtk_file_filter_new();
	gtk_file_filter_set_name( filter , _("Audio Files") );
	gtk_file_filter_add_pattern(filter , "*.wav" );
	gtk_file_filter_add_pattern(filter , "*.ul" );
	gtk_file_filter_add_pattern(filter , "*.au" );
	gtk_file_chooser_add_filter( GTK_FILE_CHOOSER( fileChooser ) , filter);
	gtk_box_pack_start( GTK_BOX(box) , fileChooser , TRUE , TRUE , 1);

	// Show all
	gtk_widget_show_all(ret);

	return ret;
}

GtkWidget*
create_general_settings ()
{
  GtkWidget *ret;

  GtkWidget *notifFrame;
  GtkWidget *notifBox;
  GtkWidget *notifAll;
  GtkWidget *notifMails;

  GtkWidget *trayFrame;
  GtkWidget *trayBox;
  GtkWidget *trayItem;

  GtkWidget *historyFrame;
  GtkWidget *historyBox;
  GtkWidget *value;
  GtkWidget *label;
  GtkWidget *cleanButton;

  // Main widget
  ret = gtk_vbox_new(FALSE, 10);
  gtk_container_set_border_width(GTK_CONTAINER(ret), 10);

  // Notifications Frame
  notifFrame = gtk_frame_new(_("Desktop Notification"));
  gtk_box_pack_start(GTK_BOX(ret), notifFrame, FALSE, FALSE, 0);
  gtk_widget_show( notifFrame );

  notifBox = gtk_vbox_new(FALSE, 10);
  gtk_box_pack_start(GTK_BOX(notifFrame), notifBox, FALSE, FALSE, 0);
  gtk_widget_show( notifBox );
  gtk_container_add( GTK_CONTAINER(notifFrame) , notifBox);
  
  notifAll = gtk_check_button_new_with_label( _("Enable"));
  gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(notifAll), dbus_get_notify() );
  gtk_box_pack_start( GTK_BOX(notifBox) , notifAll , TRUE , TRUE , 1);
  g_signal_connect(G_OBJECT( notifAll ) , "clicked" , G_CALLBACK( set_notif_level ) , NULL );

  notifMails = gtk_check_button_new_with_label(  _("Notify Voice Mails"));
  gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(notifMails), dbus_get_mail_notify() );
  gtk_box_pack_start( GTK_BOX(notifBox) , notifMails , TRUE , TRUE , 1);
  g_signal_connect(G_OBJECT( notifMails ) , "clicked" , G_CALLBACK( set_mail_notif ) , NULL);

  // System Tray option frame
  trayFrame = gtk_frame_new(_("System Tray Icon"));
  gtk_box_pack_start(GTK_BOX(ret), trayFrame, FALSE, FALSE, 0);
  gtk_widget_show( trayFrame );

  trayBox = gtk_vbox_new(FALSE, 10);
  gtk_box_pack_start(GTK_BOX(trayFrame), trayBox, FALSE, FALSE, 0);
  gtk_widget_show( trayBox );
  gtk_container_add( GTK_CONTAINER(trayFrame) , trayBox);
  
  GtkWidget* trayItem1 = gtk_radio_button_new_with_label(NULL,  _("Popup Main Window On Incoming Call"));
  gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(trayItem1), dbus_popup_mode() );
  gtk_box_pack_start( GTK_BOX(trayBox) , trayItem1 , TRUE , TRUE , 1);
  g_signal_connect(G_OBJECT( trayItem1 ) , "clicked" , G_CALLBACK( set_popup_mode ) , NULL);

  trayItem = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(trayItem1), _("Never Popup Main Window"));
  gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(trayItem), !dbus_popup_mode() );
  gtk_box_pack_start( GTK_BOX(trayBox) , trayItem , TRUE , TRUE , 1);
  
  trayItem = gtk_check_button_new_with_label(_("Start Hidden"));
  gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(trayItem), dbus_is_start_hidden() );
  gtk_box_pack_start( GTK_BOX(trayBox) , trayItem , TRUE , TRUE , 1);
  g_signal_connect(G_OBJECT( trayItem ) , "clicked" , G_CALLBACK( start_hidden ) , NULL);

  historyFrame = gtk_frame_new(_("Calls History"));
  gtk_box_pack_start(GTK_BOX(ret), historyFrame, FALSE, FALSE, 0);
  gtk_widget_show( historyFrame );

  historyBox = gtk_vbox_new(FALSE, 10);
  gtk_box_pack_start(GTK_BOX(historyFrame), historyBox, TRUE, TRUE, 0);
  gtk_widget_show( historyBox );
  gtk_container_add( GTK_CONTAINER(historyFrame) , historyBox);
  
  label = gtk_label_new_with_mnemonic(_("Maximum number of calls"));
  gtk_box_pack_start( GTK_BOX(historyBox) , label , TRUE , TRUE , 0);
  
  value = gtk_hscale_new_with_range(0.0 , 50.0 , 5.0);
  gtk_label_set_mnemonic_widget (GTK_LABEL (label), value);
  gtk_scale_set_digits( GTK_SCALE(value) , 0);
  gtk_scale_set_value_pos( GTK_SCALE(value) , GTK_POS_RIGHT); 
  gtk_range_set_value( GTK_RANGE( value ) , dbus_get_max_calls());
  gtk_box_pack_start( GTK_BOX(historyBox) , value , TRUE , TRUE , 0);
  g_signal_connect( G_OBJECT( value) , "value-changed" , G_CALLBACK( update_max_value ) , NULL);

  cleanButton = gtk_button_new_from_stock( GTK_STOCK_CLEAR );
  gtk_box_pack_end( GTK_BOX(historyBox) , cleanButton , FALSE , TRUE , 0);
  g_signal_connect( G_OBJECT( cleanButton ) , "clicked" , G_CALLBACK( clean_history ) , NULL);
  
  gtk_widget_show_all(ret);
  
  return ret;
}

/**
 * Video settings tab
 */
GtkWidget*
create_video_tab ()
{
	GtkWidget *ret;

	GtkWidget *bitrateLabel;
	GtkWidget *codecBox, *videoBox;
	GtkWidget *codecFrame, *videoFrame;
	GtkCellRenderer *bitrateRenderer;
	GtkWidget *bitrateTable;
	
	GtkWidget *codecTable;
	
	// Main widget
	ret = gtk_vbox_new(FALSE, 10);
    gtk_container_set_border_width(GTK_CONTAINER(ret), 10);

	// Codec section label
	codecFrame = gtk_frame_new("Codecs");
	gtk_box_pack_start(GTK_BOX(ret), codecFrame, FALSE, FALSE, 0);
	gtk_widget_show(codecFrame);

    // Main codec widget
	codecBox = gtk_hbox_new(FALSE, 10);
	gtk_box_pack_start(GTK_BOX(codecFrame), codecBox, FALSE, FALSE, 0);
	gtk_widget_show(codecBox);
	
	gtk_container_add(GTK_CONTAINER(codecFrame),codecBox);
	
	// Codec : List
	codecTable = create_video_codec_table();
	gtk_widget_set_size_request(GTK_WIDGET(codecTable), -1, 150);
	gtk_box_pack_start(GTK_BOX(codecBox), codecTable, TRUE, TRUE, 0);
	gtk_widget_show(codecTable);
	
	videoFrame = gtk_frame_new("Video Capture");
	gtk_box_pack_start(GTK_BOX(ret), videoFrame, FALSE, FALSE, 0);
	gtk_widget_show(videoFrame);
	
	videoBox = gtk_vbox_new(FALSE, 10);
	gtk_box_pack_start(GTK_BOX(videoFrame), videoBox, FALSE, FALSE, 0);
	gtk_widget_show(videoBox);	
	gtk_container_add(GTK_CONTAINER(videoFrame),videoBox);
	
	//Table to limit space taken by the combo box
	bitrateTable = gtk_table_new(2,3,TRUE);
    gtk_table_set_col_spacing(GTK_TABLE(bitrateTable), 0, 40);
	gtk_box_set_spacing(GTK_BOX(bitrateTable), 0);
	gtk_box_pack_start(GTK_BOX(videoBox), bitrateTable, TRUE, TRUE, 0);
	gtk_widget_show(bitrateTable);
	gtk_container_add(GTK_CONTAINER(videoBox),bitrateTable);
	
	//Bitrate combo box section
	bitrateLabel = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(bitrateLabel), "<b>Bitrate: (kbps)</b>");
    gtk_label_set_line_wrap(GTK_LABEL(bitrateLabel), TRUE);
    gtk_misc_set_alignment(GTK_MISC(bitrateLabel), 0, 0.5);
    gtk_label_set_justify(GTK_LABEL(bitrateLabel), GTK_JUSTIFY_LEFT);
    gtk_table_attach(GTK_TABLE(bitrateTable), bitrateLabel, 0, 1, 0, 1, GTK_FILL | GTK_EXPAND, GTK_SHRINK, 0, 0);
    gtk_widget_show(bitrateLabel);
    
    bitrateStore = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
	config_window_fill_bitrate_list();
	bitrateComboBox = gtk_combo_box_new_with_model(GTK_TREE_MODEL(bitrateStore));
	select_active_bitrate();
  	gtk_label_set_mnemonic_widget(GTK_LABEL(bitrateLabel), bitrateComboBox);
	g_signal_connect(G_OBJECT(bitrateComboBox), "changed", G_CALLBACK(select_bitrate), bitrateComboBox);
	
	bitrateRenderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(bitrateComboBox), bitrateRenderer, FALSE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(bitrateComboBox), bitrateRenderer, "text", 0, "text", 1, NULL);
	gtk_table_attach(GTK_TABLE(bitrateTable), bitrateComboBox, 0, 1, 1, 2, GTK_FILL | GTK_EXPAND, GTK_SHRINK, 0, 0);
    gtk_widget_show(bitrateComboBox);
	
	activateCheckBox = gtk_check_button_new_with_label("Always ask before activating the video capture");
	gtk_box_pack_start(GTK_BOX(videoBox), activateCheckBox, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(activateCheckBox), "toggled", G_CALLBACK (activate_checkbox), NULL);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(activateCheckBox), enableStatus);
	gtk_widget_show(activateCheckBox);
	
	cancelCheckBox = gtk_check_button_new_with_label("Always ask before cancelling the video capture");
	gtk_box_pack_start(GTK_BOX(videoBox), cancelCheckBox, FALSE, FALSE, 0);
	g_signal_connect(G_OBJECT(cancelCheckBox), "toggled", G_CALLBACK (cancel_checkbox), NULL);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cancelCheckBox), disableStatus);
	gtk_widget_show(cancelCheckBox);
	

	// Show all
	gtk_widget_show_all(ret);

	return ret;
}

/**
 * Webcam settings tab
 */
GtkWidget*
create_webcam_tab ()
{
	GtkWidget *ret;
	
	GtkWidget *deviceFrame, *settingsFrame;
	GtkWidget *deviceBox;
	GtkWidget *deviceTable;
	
	GtkWidget *titleLabel;
	GtkWidget *refreshButton;
	
	GtkWidget *settingsHBox;
	GtkWidget *settingsTable;
	GtkWidget *brightnessLabel, *resolutionLabel;
	GtkWidget *contrastLabel, *colourLabel;
	GtkWidget *brightnessHScale, *contrastHScale, *colourHScale;
	
	GtkWidget *drawingSpace;
	slider_t values;
	
	GtkCellRenderer *deviceRenderer, *resolutionRenderer;
	
	// Main widget
	ret = gtk_vbox_new(FALSE, 10);
    gtk_container_set_border_width(GTK_CONTAINER(ret), 10);
    
    // Device section
    deviceFrame = gtk_frame_new("Devices");
	gtk_box_pack_start(GTK_BOX(ret), deviceFrame, FALSE, FALSE, 0);
	gtk_widget_show(deviceFrame);
	
    // Main device widget
	deviceBox = gtk_hbox_new(FALSE, 10);
	gtk_box_pack_start(GTK_BOX(deviceFrame), deviceBox, FALSE, FALSE, 0);
	gtk_container_add(GTK_CONTAINER(deviceFrame),deviceBox);
    
    // Main device widget
	deviceTable = gtk_table_new(4, 3, FALSE);
	gtk_table_set_col_spacing(GTK_TABLE(deviceTable), 0, 40);
	gtk_box_set_spacing(GTK_BOX(deviceTable), 0);
	gtk_box_pack_start(GTK_BOX(deviceBox), deviceTable, TRUE, TRUE, 0);
	gtk_widget_show(deviceTable);
	
	// Device : Webcam
	// Create title label
	titleLabel = gtk_label_new("Webcam: ");
    gtk_misc_set_alignment(GTK_MISC(titleLabel), 0, 0.5);
	gtk_table_attach(GTK_TABLE(deviceTable), titleLabel, 1, 2, 0, 1, GTK_FILL | GTK_EXPAND, GTK_SHRINK, 0, 0);
	gtk_widget_show(titleLabel);	
	
	// Set choices of webcam
	webcamDeviceStore = gtk_list_store_new(1, G_TYPE_STRING);
	config_window_fill_webcam_device_list();
	webcamDeviceComboBox = gtk_combo_box_new_with_model(GTK_TREE_MODEL(webcamDeviceStore));
	select_active_webcam_device();
  	gtk_label_set_mnemonic_widget(GTK_LABEL(titleLabel), webcamDeviceComboBox);
	g_signal_connect(G_OBJECT(webcamDeviceComboBox), "changed", G_CALLBACK(select_webcam_device), webcamDeviceComboBox);
	
  	// Set rendering
	deviceRenderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(webcamDeviceComboBox), deviceRenderer, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(webcamDeviceComboBox), deviceRenderer, "text", 0, NULL);
	gtk_table_attach(GTK_TABLE(deviceTable), webcamDeviceComboBox, 2, 3, 0, 1, GTK_FILL | GTK_EXPAND, GTK_SHRINK, 0, 0);
	gtk_widget_show(webcamDeviceComboBox);
	
	// Create detect button
	refreshButton = gtk_button_new_with_label("Detect all");
	gtk_button_set_image(GTK_BUTTON(refreshButton), gtk_image_new_from_stock(GTK_STOCK_REFRESH, GTK_ICON_SIZE_BUTTON));
	gtk_table_attach(GTK_TABLE(deviceTable), refreshButton, 3, 4, 0, 3, GTK_EXPAND, GTK_EXPAND, 0, 0);
	// Set event on selection
	g_signal_connect(G_OBJECT(refreshButton), "clicked", G_CALLBACK(detect_webcam_device), NULL);
	
    // Settings section
	
    settingsHBox = gtk_hbox_new (TRUE, 10);
    gtk_box_pack_start (GTK_BOX (ret), settingsHBox, TRUE, TRUE, 0);
    
    settingsFrame = gtk_frame_new("Settings");
	gtk_box_pack_start(GTK_BOX(settingsHBox), settingsFrame, TRUE, TRUE, 0);
	gtk_widget_show(settingsFrame);
    
    settingsTable = gtk_table_new(8,1,TRUE);
    gtk_table_set_col_spacing(GTK_TABLE(settingsTable), 0, 40);
	gtk_box_set_spacing(GTK_BOX(settingsTable), 0);
	gtk_box_pack_start(GTK_BOX(settingsFrame), settingsTable, TRUE, TRUE, 0);
	gtk_widget_show(settingsTable);
	gtk_container_add(GTK_CONTAINER(settingsFrame),settingsTable);
	
	//Get the values for the brightness slider
	values = dbus_get_brightness();
	printf("load brightness to: %d \n", values.currentValue);
    
    //Brightness slider section
    brightnessLabel = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(brightnessLabel), "<b>Brightness:</b>");
    gtk_label_set_line_wrap(GTK_LABEL(brightnessLabel), TRUE);
    gtk_misc_set_alignment(GTK_MISC(brightnessLabel), 0, 0.5);
    gtk_label_set_justify(GTK_LABEL(brightnessLabel), GTK_JUSTIFY_LEFT);
    gtk_table_attach(GTK_TABLE(settingsTable), brightnessLabel, 0, 1, 0, 1, GTK_FILL | GTK_EXPAND, GTK_SHRINK, 0, 0);
    gtk_widget_show(brightnessLabel);

	brightnessAdjustment = gtk_adjustment_new (values.currentValue, values.minValue, values.maxValue, values.stepValue, 1, 1);
    brightnessHScale = gtk_hscale_new(GTK_ADJUSTMENT (brightnessAdjustment));
    gtk_scale_set_digits(GTK_SCALE(brightnessHScale), 0);
    gtk_table_attach(GTK_TABLE(settingsTable), brightnessHScale, 0, 1, 1, 2, GTK_FILL | GTK_EXPAND, GTK_SHRINK, 0, 0);
    g_signal_connect(G_OBJECT(brightnessHScale), "value-changed", G_CALLBACK(set_brightness), brightnessHScale);
    if(values.currentValue ==-1 && values.minValue==-1 && values.maxValue==-1 && values.stepValue==-1)
	{
    	gtk_widget_set_sensitive(GTK_WIDGET(brightnessHScale), 0);
	}
	gtk_widget_show(brightnessHScale);
	
	//Get the values for the contrast slider
	values = dbus_get_contrast();

	//Contrast slider section
    contrastLabel = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(contrastLabel), "<b>Contrast:</b>");
    gtk_label_set_line_wrap(GTK_LABEL(contrastLabel), TRUE);
    gtk_misc_set_alignment(GTK_MISC(contrastLabel), 0, 0.5);
    gtk_label_set_justify(GTK_LABEL(contrastLabel), GTK_JUSTIFY_LEFT);
    gtk_table_attach(GTK_TABLE(settingsTable), contrastLabel, 0, 1, 2, 3, GTK_FILL | GTK_EXPAND, GTK_SHRINK, 0, 0);
    gtk_widget_show(contrastLabel);
    
    contrastAdjustment = gtk_adjustment_new (values.currentValue, values.minValue, values.maxValue, values.stepValue, 1, 1);
    contrastHScale = gtk_hscale_new(GTK_ADJUSTMENT (contrastAdjustment));
    gtk_scale_set_digits(GTK_SCALE(contrastHScale), 0);
    gtk_table_attach(GTK_TABLE(settingsTable), contrastHScale, 0, 1, 3, 4, GTK_FILL | GTK_EXPAND, GTK_SHRINK, 0, 0);
    g_signal_connect(G_OBJECT(contrastHScale), "value-changed", G_CALLBACK(set_contrast), contrastHScale);
    if(values.currentValue ==-1 && values.minValue==-1 && values.maxValue==-1 && values.stepValue==-1)
	{
    	gtk_widget_set_sensitive(GTK_WIDGET(contrastHScale), 0);
	}
	gtk_widget_show(contrastHScale);
	
	//Get the values for the colour slider
	values = dbus_get_colour();
	
	//Colour slider section
	colourLabel = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(colourLabel), "<b>Colour:</b>");
    gtk_label_set_line_wrap(GTK_LABEL(colourLabel), TRUE);
    gtk_misc_set_alignment(GTK_MISC(colourLabel), 0, 0.5);
    gtk_label_set_justify(GTK_LABEL(colourLabel), GTK_JUSTIFY_LEFT);
    gtk_table_attach(GTK_TABLE(settingsTable), colourLabel, 0, 1, 4, 5, GTK_FILL | GTK_EXPAND, GTK_SHRINK, 0, 0);
    gtk_widget_show(colourLabel);
    
    colourAdjustment = gtk_adjustment_new (values.currentValue, values.minValue, values.maxValue, values.stepValue, 1, 1);
    colourHScale = gtk_hscale_new(GTK_ADJUSTMENT (colourAdjustment));
    gtk_scale_set_digits(GTK_SCALE(colourHScale), 0);
    gtk_table_attach(GTK_TABLE(settingsTable), colourHScale, 0, 1, 5, 6, GTK_FILL | GTK_EXPAND, GTK_SHRINK, 0, 0);
    g_signal_connect(G_OBJECT(colourHScale), "value-changed", G_CALLBACK(set_colour), colourHScale);
    if(values.currentValue ==-1 && values.minValue==-1 && values.maxValue==-1 && values.stepValue==-1)
	{
    	gtk_widget_set_sensitive(GTK_WIDGET(colourHScale), 0);
	}
	gtk_widget_show(colourHScale);
	
	//Resolution combo box section
	resolutionLabel = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(resolutionLabel), "<b>Resolution:</b>");
    gtk_label_set_line_wrap(GTK_LABEL(resolutionLabel), TRUE);
    gtk_misc_set_alignment(GTK_MISC(resolutionLabel), 0, 0.5);
    gtk_label_set_justify(GTK_LABEL(resolutionLabel), GTK_JUSTIFY_LEFT);
    gtk_table_attach(GTK_TABLE(settingsTable), resolutionLabel, 0, 1, 6, 7, GTK_FILL | GTK_EXPAND, GTK_SHRINK, 0, 0);
    gtk_widget_show(resolutionLabel);
    
    resolutionStore = gtk_list_store_new(1, G_TYPE_STRING);
	config_window_fill_resolution_list();
	resolutionComboBox = gtk_combo_box_new_with_model(GTK_TREE_MODEL(resolutionStore));
	select_active_resolution();
  	gtk_label_set_mnemonic_widget(GTK_LABEL(resolutionLabel), resolutionComboBox);
	g_signal_connect(G_OBJECT(resolutionComboBox), "changed", G_CALLBACK(select_resolution), resolutionComboBox);
	
	resolutionRenderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(resolutionComboBox), resolutionRenderer, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(resolutionComboBox), resolutionRenderer, "text", 0, NULL);
	gtk_table_attach(GTK_TABLE(settingsTable), resolutionComboBox, 0, 1, 7, 8, GTK_FILL | GTK_EXPAND, GTK_SHRINK, 0, 0);
	gtk_widget_show(resolutionComboBox);
	
	// OpenGL widget to show the local video rendering
    drawingSpace= createGLWidget( TRUE );
    gtk_box_pack_start(GTK_BOX(settingsHBox), drawingSpace, TRUE, TRUE, 0);
    gtk_widget_show(drawingSpace);
	
	//g_signal_connect (G_OBJECT (colourHScale), "format-value", G_CALLBACK (format_percentage_scale), NULL); 
	//g_signal_connect (G_OBJECT (brightnessHScale), "format-value", G_CALLBACK (format_percentage_scale), NULL); 
	//g_signal_connect (G_OBJECT (contrastHScale), "format-value", G_CALLBACK (format_percentage_scale), NULL); 

	// Show all
	gtk_widget_show_all(ret);

	return ret;
}

/**
 * Show configuration window with tabs
 * page_num indicates the current page of the notebook
 * set update to true only if you want to destroy the dialog window and recreate it
 */
void
show_config_window (gint page_num)
{
	GtkWidget * tabAccount, *tabGeneral, *tabAudio, *tabVideo, *tabWebcam;

	dialogOpen = TRUE;

	dialog = GTK_DIALOG(gtk_dialog_new_with_buttons (_("Preferences"),
				GTK_WINDOW(get_main_window()),
				GTK_DIALOG_DESTROY_WITH_PARENT,
				GTK_STOCK_CLOSE,
				GTK_RESPONSE_ACCEPT,
				NULL));

	// Set window properties
    gtk_dialog_set_has_separator(dialog, FALSE);
	gtk_window_set_default_size(GTK_WINDOW(dialog), 400, 400);
	gtk_container_set_border_width(GTK_CONTAINER(dialog), 0);
	
	// Create tabs container
	notebook = gtk_notebook_new();
	gtk_box_pack_start(GTK_BOX (dialog->vbox), notebook, TRUE, TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(notebook), 10);
	gtk_widget_show(notebook);

	// Accounts tab
	tabAccount = create_accounts_tab();
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), tabAccount, gtk_label_new("Accounts"));
	gtk_notebook_page_num(GTK_NOTEBOOK(notebook), tabAccount);
	gtk_widget_show(tabAccount);

	// General settings tab
	tabGeneral = create_general_settings();
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), tabGeneral, gtk_label_new(_("General Settings")));
	gtk_notebook_page_num(GTK_NOTEBOOK(notebook), tabGeneral);

	
	// Audio tab
	tabAudio = create_audio_tab();	
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), tabAudio, gtk_label_new("Audio Settings"));
	gtk_notebook_page_num(GTK_NOTEBOOK(notebook), tabAudio);
	gtk_widget_show(tabAudio);
	
	// Video tab
	tabVideo = create_video_tab();	
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), tabVideo, gtk_label_new("Video Settings"));
	gtk_notebook_page_num(GTK_NOTEBOOK(notebook), tabVideo);
	gtk_widget_show(tabVideo);
	
	// Webcam tab
	tabWebcam = create_webcam_tab();	
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), tabWebcam, gtk_label_new("Webcam Settings"));
	gtk_notebook_page_num(GTK_NOTEBOOK(notebook), tabWebcam);
	gtk_widget_show(tabWebcam);
	
	g_signal_connect_after(G_OBJECT(notebook), "switch-page", G_CALLBACK(select_notebook_page), notebook);
	
	gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook),page_num);

	gtk_dialog_run(dialog);
	
	//g_signal_connect_swapped( dialog , "response" , G_CALLBACK( gtk_widget_destroy ), dialog );
	//gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), _("Preferences"));
	
	dbus_disable_local_video_pref();
	
	dialogOpen = FALSE;

	gtk_widget_destroy(GTK_WIDGET(dialog));
}

//Updates the webcam settings with the new values
void update_notebook()
{
	gtk_dialog_response(dialog, GTK_RESPONSE_DELETE_EVENT);
	gtk_widget_destroy(GTK_WIDGET(dialog));
	gtk_widget_destroy(GTK_WIDGET(notebook));
	printf("dialog destroyed \n");
	
	show_config_window(4);	
}

/*
 * Show accounts tab in a different window
 */
void
show_accounts_window( void )
{
	GtkDialog * dialog;
	GtkWidget * accountFrame;
	GtkWidget * tab;

	dialogOpen = TRUE;

	dialog = GTK_DIALOG(gtk_dialog_new_with_buttons (_("Accounts"),
			GTK_WINDOW(get_main_window()),
			GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_STOCK_CLOSE,
			GTK_RESPONSE_ACCEPT,
			NULL));

	// Set window properties
	gtk_dialog_set_has_separator(dialog, FALSE);
	gtk_window_set_default_size(GTK_WINDOW(dialog), 400, 400);
	gtk_container_set_border_width(GTK_CONTAINER(dialog), 0);

	accountFrame = gtk_frame_new( _("Accounts previously setup"));
	gtk_box_pack_start( GTK_BOX( dialog->vbox ), accountFrame , TRUE, TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(accountFrame), 10);
	gtk_widget_show(accountFrame);

	// Accounts tab
	tab = create_accounts_tab();

	gtk_container_add(GTK_CONTAINER(accountFrame) , tab);

	gtk_dialog_run( dialog );

	dialogOpen=FALSE;
	gtk_widget_destroy(GTK_WIDGET(dialog));
	if( account_list_get_size() >0 && account_list_get_current()==NULL ) 
		account_list_set_current_pos(0);
	toolbar_update_buttons();
}

