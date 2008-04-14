/*
 *  Copyright (C) 2007 Savoir-Faire Linux inc.
 *  Author: Pierre-Luc Beaudoin <pierre-luc@squidy.info>
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

#ifndef __CONFIGWINDOW_H__
#define __CONFIGWINDOW_H__

#include <calllist.h>

typedef struct  {
  /** Minimum value for the slider */
  int minValue;
  /** Maximum value for the slider */
  int maxValue;
  /** Step increment value for the slider */
  int stepValue;
  /** Current value of the slider */
  int currentValue;
} slider_t;

/**
 * @file configwindow.h
 * @brief The Preferences window.
 */
void config_window_fill_account_list();
void config_window_fill_codec_list();
void config_window_fill_video_codec_list();
void config_window_fill_input_audio_plugin_list();
void config_window_fill_output_audio_plugin_list();
void config_window_fill_output_audio_device_list();
void select_active_output_audio_device();
void config_window_fill_input_audio_device_list();
void select_active_input_audio_device();
void select_active_output_audio_plugin();

void config_window_fill_webcam_device_list();
void select_active_webcam_device();
void select_webcam(GtkComboBox* widget, gpointer data);

void config_window_fill_resolution_list();
void select_active_resolution();

void config_window_fill_bitrate_list();
void select_active_bitrate();

void update_combo_box( gchar* plugin );
void default_account(GtkWidget *widget, gpointer data);
void bold_if_default_account(GtkTreeViewColumn *col, GtkCellRenderer *rend, GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data);
void default_codecs(GtkWidget* widget, gpointer data);
GtkWidget * create_codec_table();
GtkWidget * create_accounts_tab();
GtkWidget * create_audio_tab();
GtkWidget * create_video_tab();
GtkWidget * create_webcam_tab();
void show_config_window(gint page_num);
void update_notebook();
void show_accounts_window();

gboolean get_enable_webcam_checkbox_status();
gboolean get_disable_webcam_checkbox_status();
void set_enable_webcam_checkbox_status(gboolean status);
void set_disable_webcam_checkbox_status(gboolean status);
void video_settings_checkbox_init();

#endif 
