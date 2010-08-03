/*
 *  Copyright (C) 2004, 2005, 2006, 2009, 2008, 2009, 2010 Savoir-Faire Linux Inc.
 *  Author: Emmanuel Milou <emmanuel.milou@savoirfairelinux.com>
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
 *
 *  Additional permission under GNU GPL version 3 section 7:
 *
 *  If you modify this program, or any covered work, by linking or
 *  combining it with the OpenSSL project's OpenSSL library (or a
 *  modified version of that library), containing parts covered by the
 *  terms of the OpenSSL or SSLeay licenses, Savoir-Faire Linux Inc.
 *  grants you additional permission to convey the resulting work.
 *  Corresponding Source for a non-source form of such a combination
 *  shall include the source code for the parts of OpenSSL used as well
 *  as that of the covered work.
 */

#include <audioconf.h>

#include <utils.h>

#include "account.h"

#include "CodecList.h"

#include <string.h>

GtkListStore *pluginlist;
GtkListStore *outputlist;
GtkListStore *inputlist;
GtkListStore *ringtonelist;

GtkWidget *output;
GtkWidget *input;
GtkWidget *ringtone;
GtkWidget *plugin;
GtkWidget *pulse;
GtkWidget *alsabox;
GtkWidget *alsa_conf;
GtkWidget *noisebox;
GtkWidget *noise_conf;

/**
 * Fill store with output audio plugins
 */
void
preferences_dialog_fill_audio_plugin_list ()
{
  GtkTreeIter iter;
  gchar** list;
  gchar* managerName;

  gtk_list_store_clear (pluginlist);

  // Call dbus to retreive list
  list = dbus_get_audio_plugin_list ();
  // For each API name included in list
  int c = 0;

  if (list != NULL)
    {
      for (managerName = list[c]; managerName != NULL; managerName = list[c])
        {
          c++;
          gtk_list_store_append (pluginlist, &iter);
          gtk_list_store_set (pluginlist, &iter, 0, managerName, -1);
        }
    }
  list = NULL;
}

/**
 * Fill output audio device store
 */
void
preferences_dialog_fill_output_audio_device_list ()
{

  GtkTreeIter iter;
  gchar** list;
  gchar** audioDevice;
  int index;

  gtk_list_store_clear (outputlist);

  // Call dbus to retreive list
  list = dbus_get_audio_output_device_list ();

  // For each device name included in list
  int c = 0;
  for (audioDevice = list; *list; list++)
    {
      index = dbus_get_audio_device_index (*list);
      gtk_list_store_append (outputlist, &iter);
      gtk_list_store_set (outputlist, &iter, 0, *list, 1, index, -1);
      c++;
    }
}

/**
 * Fill rigntone audio device store
 */

void
preferences_dialog_fill_ringtone_audio_device_list ()
{

  GtkTreeIter iter;
  gchar** list;
  gchar** audioDevice;
  int index;

  gtk_list_store_clear (ringtonelist);

  // Call dbus to retreive output device
  list = dbus_get_audio_output_device_list ();

  // For each device name in the list
  int c = 0;
  for (audioDevice = list; *list; list++)
    {
      index = dbus_get_audio_device_index (*list);
      gtk_list_store_append (ringtonelist, &iter);
      gtk_list_store_set (ringtonelist, &iter, 0, *list, 1, index, -1);
      c++;
    }
}

/**
 * Select active output audio device
 */
void
select_active_output_audio_device ()
{
  if (SHOW_ALSA_CONF)
    {

      GtkTreeModel* model;
      GtkTreeIter iter;
      gchar** devices;
      int currentDeviceIndex;
      int deviceIndex;

      // Select active output device on server
      devices = dbus_get_current_audio_devices_index ();
      currentDeviceIndex = atoi (devices[0]);
      DEBUG("audio device index for output = %d", currentDeviceIndex);
      model = gtk_combo_box_get_model (GTK_COMBO_BOX(output));

      // Find the currently set output device
      gtk_tree_model_get_iter_first (model, &iter);
      do
        {
          gtk_tree_model_get (model, &iter, 1, &deviceIndex, -1);
          if (deviceIndex == currentDeviceIndex)
            {
              // Set current iteration the active one
              gtk_combo_box_set_active_iter (GTK_COMBO_BOX(output), &iter);
              return;
            }
        }
      while (gtk_tree_model_iter_next (model, &iter));

      // No index was found, select first one
      WARN("Warning : No active output device found");
      gtk_combo_box_set_active (GTK_COMBO_BOX(output), 0);
    }
}

/**
 * Select active output audio device
 */
void
select_active_ringtone_audio_device ()
{
  if (SHOW_ALSA_CONF)
    {

      GtkTreeModel* model;
      GtkTreeIter iter;
      gchar** devices;
      int currentDeviceIndex;
      int deviceIndex;

      // Select active ringtone device on server
      devices = dbus_get_current_audio_devices_index ();
      currentDeviceIndex = atoi (devices[2]);
      DEBUG("audio device index for ringtone = %d", currentDeviceIndex);
      model = gtk_combo_box_get_model (GTK_COMBO_BOX(ringtone));

      // Find the currently set ringtone device
      gtk_tree_model_get_iter_first (model, &iter);
      do
        {
          gtk_tree_model_get (model, &iter, 1, &deviceIndex, -1);
          if (deviceIndex == currentDeviceIndex)
            {
              // Set current iteration the active one
              gtk_combo_box_set_active_iter (GTK_COMBO_BOX(ringtone), &iter);
              return;
            }
        }
      while (gtk_tree_model_iter_next (model, &iter));

      // No index was found, select first one
      WARN("Warning : No active ringtone device found");
      gtk_combo_box_set_active (GTK_COMBO_BOX(ringtone), 0);
    }
}

/**
 * Fill input audio device store
 */
void
preferences_dialog_fill_input_audio_device_list ()
{

  GtkTreeIter iter;
  gchar** list;
  gchar** audioDevice;
  int index;
  gtk_list_store_clear (inputlist);

  // Call dbus to retreive list
  list = dbus_get_audio_input_device_list ();

  // For each device name included in list
  //int c = 0;
  for (audioDevice = list; *list; list++)
    {
      index = dbus_get_audio_device_index (*list);
      gtk_list_store_append (inputlist, &iter);
      gtk_list_store_set (inputlist, &iter, 0, *list, 1, index, -1);
      //c++;
    }

}

/**
 * Select active input audio device
 */
void
select_active_input_audio_device ()
{
  if (SHOW_ALSA_CONF)
    {

      GtkTreeModel* model;
      GtkTreeIter iter;
      gchar** devices;
      int currentDeviceIndex;
      int deviceIndex;

      // Select active input device on server
      devices = dbus_get_current_audio_devices_index ();
      currentDeviceIndex = atoi (devices[1]);
      model = gtk_combo_box_get_model (GTK_COMBO_BOX(input));

      // Find the currently set input device
      gtk_tree_model_get_iter_first (model, &iter);
      do
        {
          gtk_tree_model_get (model, &iter, 1, &deviceIndex, -1);
          if (deviceIndex == currentDeviceIndex)
            {
              // Set current iteration the active one
              gtk_combo_box_set_active_iter (GTK_COMBO_BOX(input), &iter);
              return;
            }
        }
      while (gtk_tree_model_iter_next (model, &iter));

      // No index was found, select first one
      WARN("Warning : No active input device found");
      gtk_combo_box_set_active (GTK_COMBO_BOX(input), 0);
    }
}

/**
 * Select the output audio plugin by calling the server
 */
static void
select_output_audio_plugin (GtkComboBox* widget, gpointer data UNUSED)
{
  GtkTreeModel* model;
  GtkTreeIter iter;
  int comboBoxIndex;
  gchar* pluginName;

  comboBoxIndex = gtk_combo_box_get_active (widget);

  if (comboBoxIndex >= 0)
    {
      model = gtk_combo_box_get_model (widget);
      gtk_combo_box_get_active_iter (widget, &iter);
      gtk_tree_model_get (model, &iter, 0, &pluginName, -1);
      dbus_set_output_audio_plugin (pluginName);
      //update_combo_box( pluginName);
    }
}

/**
 * Select active output audio plugin
 */
void
select_active_output_audio_plugin ()
{
  GtkTreeModel* model;
  GtkTreeIter iter;
  gchar* pluginname;
  gchar* tmp;

  // Select active output device on server
  pluginname = dbus_get_current_audio_output_plugin ();
  tmp = pluginname;
  model = gtk_combo_box_get_model (GTK_COMBO_BOX(plugin));

  // Find the currently alsa plugin
  gtk_tree_model_get_iter_first (model, &iter);
  do
    {
      gtk_tree_model_get (model, &iter, 0, &pluginname, -1);
      if (g_strcasecmp (tmp, pluginname) == 0)
        {
          // Set current iteration the active one
          gtk_combo_box_set_active_iter (GTK_COMBO_BOX(plugin), &iter);
          //update_combo_box( plugin );
          return;
        }
    }
  while (gtk_tree_model_iter_next (model, &iter));

  // No index was found, select first one
  WARN("Warning : No active output device found");
  gtk_combo_box_set_active (GTK_COMBO_BOX(plugin), 0);
}

/**
 * Set the audio output device on the server with its index
 */
static void
select_audio_output_device (GtkComboBox* comboBox, gpointer data UNUSED)
{
  GtkTreeModel* model;
  GtkTreeIter iter;
  int comboBoxIndex;
  int deviceIndex;

  comboBoxIndex = gtk_combo_box_get_active (comboBox);

  if (comboBoxIndex >= 0)
    {
      model = gtk_combo_box_get_model (comboBox);
      gtk_combo_box_get_active_iter (comboBox, &iter);
      gtk_tree_model_get (model, &iter, 1, &deviceIndex, -1);

      dbus_set_audio_output_device (deviceIndex);
    }
}

/**
 * Set the audio input device on the server with its index
 */
static void
select_audio_input_device (GtkComboBox* comboBox, gpointer data UNUSED)
{
  GtkTreeModel* model;
  GtkTreeIter iter;
  int comboBoxIndex;
  int deviceIndex;

  comboBoxIndex = gtk_combo_box_get_active (comboBox);

  if (comboBoxIndex >= 0)
    {
      model = gtk_combo_box_get_model (comboBox);
      gtk_combo_box_get_active_iter (comboBox, &iter);
      gtk_tree_model_get (model, &iter, 1, &deviceIndex, -1);

      dbus_set_audio_input_device (deviceIndex);
    }
}

/**
 * Set the audio ringtone device on the server with its index
 */
static void
select_audio_ringtone_device (GtkComboBox *comboBox, gpointer data UNUSED)
{
  GtkTreeModel *model;
  GtkTreeIter iter;
  int comboBoxIndex;
  int deviceIndex;

  comboBoxIndex = gtk_combo_box_get_active (comboBox);

  if (comboBoxIndex >= 0)
    {
      model = gtk_combo_box_get_model (comboBox);
      gtk_combo_box_get_active_iter (comboBox, &iter);

      gtk_tree_model_get (model, &iter, 1, &deviceIndex, -1);

      dbus_set_audio_ringtone_device (deviceIndex);
    }
}

void
select_audio_manager (void)
{
  DEBUG("audio manager selected");

  if (!SHOW_ALSA_CONF && !gtk_toggle_button_get_active (
      GTK_TOGGLE_BUTTON(pulse)))
    {
      dbus_set_audio_manager (ALSA);
      DEBUG(" display alsa conf panel");
      alsabox = alsa_box ();
      gtk_container_add (GTK_CONTAINER(alsa_conf ), alsabox);
      gtk_widget_show (alsa_conf);
      gtk_widget_set_sensitive (GTK_WIDGET(alsa_conf), TRUE);

      gtk_action_set_sensitive (GTK_ACTION (volumeToggle), TRUE);
    }
  else if (SHOW_ALSA_CONF && gtk_toggle_button_get_active (
      GTK_TOGGLE_BUTTON(pulse)))
    {
      dbus_set_audio_manager (PULSEAUDIO);
      DEBUG(" remove alsa conf panel");
      gtk_container_remove (GTK_CONTAINER(alsa_conf), alsabox);
      gtk_widget_hide (alsa_conf);
      if (gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (volumeToggle)))
        {
          main_window_volume_controls (FALSE);
          eel_gconf_set_integer (SHOW_VOLUME_CONTROLS, FALSE);
          gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (volumeToggle), FALSE);
        }
      gtk_action_set_sensitive (GTK_ACTION (volumeToggle), FALSE);
    }
  else
    {
      DEBUG("alsa conf panel...nothing");
    }

}

void
active_echo_cancel (void)
{

  gchar* state;
  gchar* newstate;

  DEBUG("Audio: Active echo cancel clicked");
  state = dbus_get_echo_cancel_state ();

  DEBUG("Audio: Get echo cancel state %s", state);

  if (strcmp (state, "enabled") == 0)
    newstate = "disabled";
  else
    newstate = "enabled";

  dbus_set_echo_cancel_state (newstate);

}

void
active_noise_suppress (void)
{

  gchar *state;
  gchar *newstate;

  DEBUG("Audio: Active noise suppress clicked");
  state = dbus_get_noise_suppress_state ();

  DEBUG("Audio: Get echo cancel state %s", state);

  if (strcmp (state, "enabled") == 0)
    newstate = "disabled";
  else
    newstate = "enabled";

  dbus_set_noise_suppress_state (newstate);

}

GtkWidget*
alsa_box ()
{
	GtkWidget *ret;
	GtkWidget *table;
	GtkWidget *item;
	GtkCellRenderer *renderer;

	ret = gtk_hbox_new(FALSE, 10);
	gtk_widget_show( ret );

	table = gtk_table_new(5, 3, FALSE);
	gtk_table_set_col_spacing(GTK_TABLE(table), 0, 40);
	gtk_box_pack_start( GTK_BOX(ret) , table , TRUE , TRUE , 1);
	gtk_widget_show(table);

	DEBUG("Audio: Configuration plugin");
	item = gtk_label_new(_("ALSA plugin"));
	gtk_misc_set_alignment(GTK_MISC(item), 0, 0.5);
	gtk_table_attach(GTK_TABLE(table), item, 1, 2, 1, 2, GTK_FILL | GTK_EXPAND, GTK_SHRINK, 0, 0);
	gtk_widget_show( item );
	// Set choices of audio managers
	pluginlist = gtk_list_store_new(1, G_TYPE_STRING);
	preferences_dialog_fill_audio_plugin_list();
	plugin = gtk_combo_box_new_with_model(GTK_TREE_MODEL(pluginlist));
	select_active_output_audio_plugin();
	gtk_label_set_mnemonic_widget(GTK_LABEL(item), plugin);
	g_signal_connect(G_OBJECT(plugin), "changed", G_CALLBACK(select_output_audio_plugin), plugin);

	// Set rendering
	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(plugin), renderer, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(plugin), renderer, "text", 0, NULL);
	gtk_table_attach(GTK_TABLE(table), plugin, 2, 3, 1, 2, GTK_FILL | GTK_EXPAND, GTK_SHRINK, 0, 0);
	gtk_widget_show(plugin);

	// Device : Output device
	// Create title label
	DEBUG("Audio: Configuration output");
	item = gtk_label_new(_("Output"));
	gtk_misc_set_alignment(GTK_MISC(item), 0, 0.5);
	gtk_table_attach(GTK_TABLE(table), item, 1, 2, 2, 3, GTK_FILL | GTK_EXPAND, GTK_SHRINK, 0, 0);
	gtk_widget_show(item);
	// Set choices of output devices
	outputlist = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_INT);
	preferences_dialog_fill_output_audio_device_list();
	output = gtk_combo_box_new_with_model(GTK_TREE_MODEL(outputlist));
	select_active_output_audio_device();
	gtk_label_set_mnemonic_widget(GTK_LABEL(item), output);
	g_signal_connect(G_OBJECT(output), "changed", G_CALLBACK(select_audio_output_device), output);

	// Set rendering
	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(output), renderer, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(output), renderer, "text", 0, NULL);
	gtk_table_attach(GTK_TABLE(table), output, 2, 3, 2, 3, GTK_FILL | GTK_EXPAND, GTK_SHRINK, 0, 0);
	gtk_widget_show(output);

	// Device : Input device
	// Create title label
	DEBUG("Audio: Configuration input");
	item = gtk_label_new(_("Input"));
	gtk_misc_set_alignment(GTK_MISC(item), 0, 0.5);
	gtk_table_attach(GTK_TABLE(table), item, 1, 2, 3, 4, GTK_FILL | GTK_EXPAND, GTK_SHRINK, 0, 0);
	gtk_widget_show(item);

	// Set choices of output devices
	inputlist = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_INT);
	preferences_dialog_fill_input_audio_device_list();
	input = gtk_combo_box_new_with_model(GTK_TREE_MODEL(inputlist));
	select_active_input_audio_device();
	gtk_label_set_mnemonic_widget(GTK_LABEL(item), input);
	g_signal_connect(G_OBJECT(input), "changed", G_CALLBACK(select_audio_input_device), input);

	// Set rendering
	renderer = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(input), renderer, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(input), renderer, "text", 0, NULL);
	gtk_table_attach(GTK_TABLE(table), input, 2, 3, 3, 4, GTK_FILL | GTK_EXPAND, GTK_SHRINK, 0, 0);
	gtk_widget_show(input);

	gtk_widget_show_all(ret);

	DEBUG("done");
	return ret;
}

static void
record_path_changed (GtkFileChooser *chooser, GtkLabel *label UNUSED)
{
  DEBUG("record_path_changed");

  gchar* path;
  path = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER( chooser ));
  DEBUG("path2 %s", path);
  dbus_set_record_path (path);
}

GtkWidget*
create_audio_configuration ()
{
	// Main widget
	GtkWidget *ret;
	// Sub boxes
	GtkWidget *box;
	GtkWidget *frame;
	GtkWidget *enableEchoCancel;
	GtkWidget *enableNoiseReduction;
	gboolean echocancelActive, noisesuppressActive;
	gchar *state;

	ret = gtk_vbox_new(FALSE, 10);
	gtk_container_set_border_width(GTK_CONTAINER(ret), 10);

	GtkWidget *alsa;
	GtkWidget *table;

	gnome_main_section_new_with_table (_("Sound Manager"), &frame, &table, 1, 2);
	gtk_box_pack_start(GTK_BOX(ret), frame, FALSE, FALSE, 0);

	int audio_manager = dbus_get_audio_manager();
	gboolean pulse_audio = FALSE;
	if (audio_manager == PULSEAUDIO) {
		pulse_audio = TRUE;
	}

	pulse = gtk_radio_button_new_with_mnemonic( NULL , _("_Pulseaudio"));
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(pulse), pulse_audio);
	gtk_table_attach ( GTK_TABLE( table ), pulse, 0, 1, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

	alsa = gtk_radio_button_new_with_mnemonic_from_widget(GTK_RADIO_BUTTON(pulse),  _("_ALSA"));
	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(alsa), !pulse_audio);
	g_signal_connect(G_OBJECT(alsa), "clicked", G_CALLBACK(select_audio_manager), NULL);
	gtk_table_attach ( GTK_TABLE( table ), alsa, 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

	// Box for the ALSA configuration
	gnome_main_section_new (_("ALSA settings"), &alsa_conf);
	gtk_box_pack_start(GTK_BOX(ret), alsa_conf, FALSE, FALSE, 0);
	gtk_widget_show( alsa_conf );
	if( SHOW_ALSA_CONF )
	{
		// Box for the ALSA configuration
		printf("ALSA Created \n");
		alsabox = alsa_box();
		gtk_container_add( GTK_CONTAINER(alsa_conf) , alsabox );
		gtk_widget_hide( alsa_conf );
	}

	// Recorded file saving path
	GtkWidget *label;
	GtkWidget *folderChooser;
	gchar *dftPath;

	/* Get the path where to save audio files */
	dftPath = dbus_get_record_path ();
	DEBUG("load recording path %s\n", dftPath);

	gnome_main_section_new_with_table (_("Recordings"), &frame, &table, 1, 2);
	gtk_box_pack_start(GTK_BOX(ret), frame, FALSE, FALSE, 0);

	// label
	label = gtk_label_new(_("Destination folder"));
	gtk_table_attach( GTK_TABLE(table), label, 0, 1, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 5);

	// folder chooser button
	folderChooser = gtk_file_chooser_button_new(_("Select a folder"), GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER( folderChooser), dftPath);
	g_signal_connect( G_OBJECT( folderChooser ) , "selection_changed" , G_CALLBACK( record_path_changed ) , NULL );
	gtk_table_attach(GTK_TABLE(table), folderChooser, 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 5);


	// Box for the voice enhancement configuration
	gnome_main_section_new_with_table (_("Voice enhancement settings"), &frame, &table, 2, 1);
    gtk_box_pack_start(GTK_BOX(ret), frame, FALSE, FALSE, 0);

	enableEchoCancel = gtk_check_button_new_with_mnemonic( _("_Echo Suppression"));
    state = dbus_get_echo_cancel_state();
    echocancelActive = FALSE;
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(enableEchoCancel),
      echocancelActive);
  g_signal_connect(G_OBJECT(enableEchoCancel), "clicked", active_echo_cancel, NULL);
  gtk_table_attach (GTK_TABLE(table), enableEchoCancel, 0, 1, 0, 1, GTK_EXPAND
      | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  enableNoiseReduction = gtk_check_button_new_with_mnemonic (
      _("_Noise Reduction"));
  state = dbus_get_noise_suppress_state ();
  noisesuppressActive = FALSE;
  if (strcmp (state, "enabled") == 0)
    noisesuppressActive = TRUE;
  else
    noisesuppressActive = FALSE;
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(enableNoiseReduction),
      noisesuppressActive);
  g_signal_connect(G_OBJECT(enableNoiseReduction), "clicked", active_noise_suppress, NULL);
  gtk_table_attach (GTK_TABLE(table), enableNoiseReduction, 0, 1, 1, 2,
      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);

  gtk_widget_show_all (ret);

  if (!pulse_audio)
    {
      gtk_widget_show (alsa_conf);
    }
  else
    {
      gtk_widget_hide (alsa_conf);
    }

  return ret;
}
