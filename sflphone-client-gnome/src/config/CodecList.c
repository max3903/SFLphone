/*
 *  Copyright (C) 2004, 2005, 2006, 2009, 2008, 2009, 2010 Savoir-Faire Linux Inc.
 *  Author: Pierre-Luc Bacon <pierre-luc.bacon@savoirfairelinux.com>
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
#include "CodecList.h"
#include "sflphone_const.h"

static const int DEFAULT_SPACING = 10;
G_DEFINE_ABSTRACT_TYPE (CodecList, codec_list, GTK_TYPE_VBOX)

#define GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), SFL_TYPE_CODEC_LIST, CodecListPrivate))

typedef struct _CodecListPrivate CodecListPrivate;

struct _CodecListPrivate
{
  GtkWidget *codec_tree_view;
  GtkTreeModel* codec_store;

  GtkWidget* codec_move_up_button;
  GtkWidget* codec_move_down_button;

  account_t* account;
};

enum
{
  COLUMN_CODEC_ACTIVE,
  COLUMN_CODEC_NAME,
  COLUMN_CODEC_FREQUENCY,
  COLUMN_CODEC_BITRATE,
  COLUMN_CODEC_BANDWIDTH,
  COLUMN_CODEC_POINTER,
  CODEC_COLUMN_COUNT
};

enum
{
  PROP_ACCOUNT = 1, LAST_PROPERTY
} CodecListProperties;

static void
codec_list_fill (CodecList* self)
{
  CodecListPrivate* priv = GET_PRIVATE(self);

  // Get model of view and clear it
  GtkListStore
      * codecStore =
          GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (priv->codec_tree_view)));
  gtk_list_store_clear (codecStore);

  // Get all the codecs for this account
  CodecListClass* class = SFL_GET_CODEC_LIST_CLASS(self);
  if (class->load_codecs)
    {
      class->load_codecs (priv->account);
    }

  GQueue* current;
  if (class->get_codecs)
    {
      current = class->get_codecs (priv->account->codecs);
    }

  guint length = g_queue_get_length (current);

  DEBUG("Queue has length : %d", length);

  // Insert in the tree view
  int i;
  GtkTreeIter iter;
  for (i = 0; i < length; i++)
    {
      codec_t* codec = g_queue_peek_nth (current, i);
      if (!codec)
        {
          DEBUG("Codec is NULL for %d", i);
          continue;
        }
      gtk_list_store_append (codecStore, &iter);

      gtk_list_store_set (codecStore, &iter, COLUMN_CODEC_ACTIVE,
          codec->audio.is_active, // Active
          COLUMN_CODEC_NAME, codec->audio.mime_subtype, // Name
          COLUMN_CODEC_FREQUENCY, g_strdup_printf ("%d Hz",
              codec->audio.clock_rate), // Frequency (kHz)
          COLUMN_CODEC_BITRATE, g_strdup_printf ("%.1f kbps",
              codec->audio.bitrate), // Bitrate (kbps)
          COLUMN_CODEC_BANDWIDTH, g_strdup_printf ("%.1f kbps",
              codec->audio.bandwidth), // Bandwidth (kpbs)
          COLUMN_CODEC_POINTER, codec, -1);
    }
}

static void
codec_list_set_property (GObject *object, guint property_id,
    const GValue *value, GParamSpec *pspec)
{
  CodecList* self = SFL_CODEC_LIST(object);
  CodecListPrivate* priv = GET_PRIVATE((CodecList*) self);

  switch (property_id)
    {
  case PROP_ACCOUNT:
    priv->account = g_value_get_pointer (value);
    ;
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
codec_list_get_property (GObject *object, guint property_id, GValue *value,
    GParamSpec *pspec)
{
  CodecList *self = SFL_CODEC_LIST(object);
  CodecListPrivate* priv = GET_PRIVATE((CodecList*) self);

  switch (property_id)
    {
  case PROP_ACCOUNT:
    g_value_set_pointer (value, priv->account);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

void
codec_list_set_account (CodecList* codec_list, account_t* account)
{
  g_object_set (G_OBJECT(codec_list), "account", account, NULL);
}

static void
codec_list_dispose (GObject *object)
{
  CodecListPrivate* priv = GET_PRIVATE((CodecList*) object);

  G_OBJECT_CLASS (codec_list_parent_class)->dispose (object);
}

static void
codec_list_finalize (GObject* object)
{
  G_OBJECT_CLASS (codec_list_parent_class)->finalize (object);
}

static void
codec_list_realize (GtkWidget* widget)
{
  DEBUG("Realizing ....");
  GTK_WIDGET_CLASS(codec_list_parent_class)->realize (widget);

  CodecListPrivate* priv = GET_PRIVATE(widget);

  if (priv->account)
    {
      codec_list_fill (SFL_CODEC_LIST(widget));
    }
}

static void
codec_list_class_init (CodecListClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = codec_list_dispose;
  object_class->get_property = codec_list_get_property;
  object_class->set_property = codec_list_set_property;
  object_class->finalize = codec_list_finalize;

  GtkWidgetClass* widget_class;
  widget_class = GTK_WIDGET_CLASS (klass);
  widget_class->realize = codec_list_realize;

  // Install properties
  g_object_class_install_property (object_class, PROP_ACCOUNT,
      g_param_spec_pointer ("account", "account",
          "The account for which to display codecs for.",
          G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));

  // Install private data
  g_type_class_add_private (klass, sizeof(CodecListPrivate));
}

static GObject*
codec_list_constructor (GType type, guint n_construct_properties,
    GObjectConstructParam *construct_properties)
{
  gint i;
  for (i = 0; i < n_construct_properties; i++)
    {
      /* we will print the the properties' names and if it's
       * an integer we'll print it's value
       */
      g_print ("%s", construct_properties[i].pspec->name);
      if (G_VALUE_HOLDS_INT (construct_properties[i].value))
        g_print (": %d", g_value_get_int (construct_properties[i].value));
      g_print ("\n");
    }
}

/**
 * Toggle move buttons on if a codec is selected, off otherwise
 */
static void
select_codec_cb (GtkTreeSelection *selection, gpointer data)
{
  GtkTreeIter iter;
  CodecListPrivate* priv = (CodecListPrivate*) data;

  if (!gtk_tree_selection_get_selected (selection, &priv->codec_store, &iter))
    {
      gtk_widget_set_sensitive (GTK_WIDGET(priv->codec_move_up_button), FALSE);
      gtk_widget_set_sensitive (GTK_WIDGET(priv->codec_move_down_button), FALSE);
    }
  else
    {
      gtk_widget_set_sensitive (GTK_WIDGET(priv->codec_move_up_button), TRUE);
      gtk_widget_set_sensitive (GTK_WIDGET(priv->codec_move_down_button), TRUE);
    }
}

/**
 * Toggle active value of codec on click and update changes to the deamon
 * and in configuration files
 */
static void
codec_active_toggled_cb (GtkCellRendererToggle *renderer, gchar *path,
    gpointer data)
{
  // Get path of clicked codec active toggle box
  GtkTreePath* treePath = gtk_tree_path_new_from_string (path);

  CodecListPrivate* priv = (CodecListPrivate*) data;
  GtkTreeModel* model = gtk_tree_view_get_model (
      GTK_TREE_VIEW (priv->codec_tree_view));

  // Retrieve userdata
  account_t *account = (account_t*) priv->account;
  if (!account)
    {
      ERROR ("No account selected when toggling codec.");
    }

  GtkTreeIter iter;
  gtk_tree_model_get_iter (model, &iter, treePath);

  // Get active value and name at iteration
  gboolean active;
  char* name;
  char* srate;
  codec_t* codec;
  gtk_tree_model_get (model, &iter, COLUMN_CODEC_ACTIVE, &active,
      COLUMN_CODEC_NAME, &name, COLUMN_CODEC_FREQUENCY, &srate,
      COLUMN_CODEC_POINTER, &codec, -1);

  // Store value
  gtk_list_store_set (GTK_LIST_STORE(model), &iter, COLUMN_CODEC_ACTIVE,
      !active, -1);
  gtk_tree_path_free (treePath);

  // Modify codec queue to represent change
  DEBUG("Toggling codec %s", codec->codec.mime_subtype);
  codec_library_toggle_active (account->codecs, codec);
}

/**
 * Move codec in list depending on direction and selected codec and
 * update changes in the daemon list and the configuration files
 */
static void
codec_move_cb (gboolean move_up, gpointer data)
{
  // Get view, model and selection of codec store
  CodecListPrivate* priv = (CodecListPrivate*) data;

  GtkTreeModel* model = gtk_tree_view_get_model (GTK_TREE_VIEW(priv->codec_tree_view));

  GtkTreeSelection* selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(priv->codec_tree_view));

  GtkTreeIter current_line_iter;
  gtk_tree_selection_get_selected (GTK_TREE_SELECTION(selection), &model, &current_line_iter);

   // Find path of iteration
  gchar *path = gtk_tree_model_get_string_from_iter (GTK_TREE_MODEL(model), &current_line_iter);

  GtkTreePath* tree_path = gtk_tree_path_new_from_string (path);

  GtkListStore* store = GTK_LIST_STORE(model);

  codec_t* codec;
  gtk_tree_model_get (model, &current_line_iter, COLUMN_CODEC_POINTER, &codec, -1);

  DEBUG("Iter selected : %s", codec->codec.mime_subtype);

  // Retrieve the user data
  account_t* account = (account_t*) priv->account;
  GtkTreeIter swapped_line_iter;
  if (move_up)
    {
      gtk_tree_path_prev (tree_path);
      gtk_tree_model_get_iter (model, &swapped_line_iter, tree_path);
      gtk_list_store_move_before(store, &current_line_iter, &swapped_line_iter);

      DEBUG("Iter after model change : %s", codec->codec.mime_subtype);

      codec_library_move_codec_up (account->codecs, codec);
    }
  else
    {
      gtk_tree_path_next (tree_path);
      gtk_tree_model_get_iter (model, &swapped_line_iter, tree_path);
      gtk_list_store_move_after(store, &current_line_iter, &swapped_line_iter);
      codec_library_move_codec_down (account->codecs, codec);
    }
}

/**
 * Called from move up codec button signal
 */
static void
codec_move_up_cb (GtkButton* button, gpointer data)
{
  // Change tree view ordering and get indice changed
  codec_move_cb (TRUE, data);
}

/**
 * Called from move down codec button signal
 */
static void
codec_move_down_cb (GtkButton *button, gpointer data)
{
  // Change tree view ordering and get indice changed
  codec_move_cb (FALSE, data);
}

static void
codec_list_init (CodecList* self)
{
  CodecListPrivate* priv = GET_PRIVATE(self);

  GtkWidget *scrolledWindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(scrolledWindow),
      GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW(scrolledWindow),
      GTK_SHADOW_IN);

  priv->codec_store = gtk_list_store_new (CODEC_COLUMN_COUNT, G_TYPE_BOOLEAN, // Active
      G_TYPE_STRING, // Name
      G_TYPE_STRING, // Frequency
      G_TYPE_STRING, // Bit rate
      G_TYPE_STRING, // Bandwith
      G_TYPE_POINTER // A pointer to the actual codec.
      );

  // Create codec tree view with list store
  priv->codec_tree_view = gtk_tree_view_new_with_model (
      GTK_TREE_MODEL(priv->codec_store));

  // Get tree selection manager
  GtkTreeSelection* treeSelection = gtk_tree_view_get_selection (
      GTK_TREE_VIEW(priv->codec_tree_view));
  g_signal_connect(G_OBJECT(treeSelection), "changed", G_CALLBACK (select_codec_cb), priv);

  // Active column
  GtkCellRenderer* renderer = gtk_cell_renderer_toggle_new ();
  GtkTreeViewColumn* treeViewColumn = gtk_tree_view_column_new_with_attributes (
      "", renderer, "active", COLUMN_CODEC_ACTIVE, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW(priv->codec_tree_view),
      treeViewColumn);

  // Toggle codec active property on clicked
  g_signal_connect(G_OBJECT(renderer), "toggled", G_CALLBACK (codec_active_toggled_cb), priv);

  // Name column
  renderer = gtk_cell_renderer_text_new ();
  treeViewColumn = gtk_tree_view_column_new_with_attributes (_("MIME Subtype"),
      renderer, "markup", COLUMN_CODEC_NAME, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW(priv->codec_tree_view),
      treeViewColumn);

  // Bit rate column
  renderer = gtk_cell_renderer_text_new ();
  treeViewColumn = gtk_tree_view_column_new_with_attributes (_("Frequency"),
      renderer, "text", COLUMN_CODEC_FREQUENCY, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW(priv->codec_tree_view),
      treeViewColumn);

  // Bandwith column
  renderer = gtk_cell_renderer_text_new ();
  treeViewColumn = gtk_tree_view_column_new_with_attributes (_("Bitrate"),
      renderer, "text", COLUMN_CODEC_BITRATE, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW(priv->codec_tree_view),
      treeViewColumn);

  // Frequency column
  renderer = gtk_cell_renderer_text_new ();
  treeViewColumn = gtk_tree_view_column_new_with_attributes (_("Bandwidth"),
      renderer, "text", COLUMN_CODEC_BANDWIDTH, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW(priv->codec_tree_view),
      treeViewColumn);

  g_object_unref (G_OBJECT(priv->codec_store));
  gtk_container_add (GTK_CONTAINER(scrolledWindow), priv->codec_tree_view);

  // Create button box
  GtkWidget* buttonBox = gtk_vbox_new (FALSE, DEFAULT_SPACING);
  gtk_container_set_border_width (GTK_CONTAINER(buttonBox), DEFAULT_SPACING);

  priv->codec_move_up_button = gtk_button_new_from_stock (GTK_STOCK_GO_UP);
  gtk_widget_set_sensitive (GTK_WIDGET(priv->codec_move_up_button), FALSE);
  gtk_box_pack_start (GTK_BOX(buttonBox), priv->codec_move_up_button, FALSE,
      FALSE, 0);
  g_signal_connect(G_OBJECT(priv->codec_move_up_button), "clicked", G_CALLBACK(codec_move_up_cb), priv);

  priv->codec_move_down_button = gtk_button_new_from_stock (GTK_STOCK_GO_DOWN);
  gtk_widget_set_sensitive (GTK_WIDGET( priv->codec_move_down_button), FALSE);
  gtk_box_pack_start (GTK_BOX(buttonBox), priv->codec_move_down_button, FALSE,
      FALSE, 0);
  g_signal_connect(G_OBJECT(priv->codec_move_down_button), "clicked", G_CALLBACK(codec_move_down_cb), priv);

  // Pack everything up
  GtkWidget* hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX(hbox), scrolledWindow, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX(hbox), buttonBox, FALSE, FALSE, 0);

  gtk_box_pack_start (GTK_BOX(self), hbox, TRUE, TRUE, 0);

  gtk_widget_show_all (GTK_WIDGET(self));
}

CodecList*
codec_list_new (account_t* account)
{
  return g_object_new (SFL_TYPE_CODEC_LIST, "account", account, NULL);
}

void
codec_list_save (CodecList* widget)
{
  CodecListPrivate* priv = GET_PRIVATE(widget);

  if (priv->account)
    {
      CodecListClass* class = SFL_GET_CODEC_LIST_CLASS(widget);
      if (class->save_codecs)
        {
          class->save_codecs (priv->account->codecs, priv->account->accountID);
        }
    }
}
