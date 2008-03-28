/*
 *  Copyright (C) 2007 Savoir-Faire Linux inc.
 *  Author: Jean Tessier <jean.tessier@polymtl.ca>
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

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <gtk/gtk.h>
#include <gtk/gtkgl.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "MemManager.h"

int currentGLWidth;
int currentGLHeight;

//! Configuration information for the gl widget
GdkGLConfig* glconfig;

//! Callback Function to draw the content of the widget
/*!
 * \param widget a pointer to the widget being drawned
 * \param data data on the call back
 * \return the success of the operation
 */
gboolean draw(GtkWidget* widget, gpointer data);

//! Callback Function to reshape the content of the widget
/*!
 * \param widget a pointer to the widget being drawned
 * \param ev a pointer to the event data
 * \param data data on the call back
 * \return the success of the operation
 */
gboolean reshape(GtkWidget* widget, GdkEventConfigure* ev, gpointer data);

//! Callback Function to initialise the content of the widget
/*!
 * \param widget a pointer to the widget being drawned
 * \param data data on the call back
 * \return the success of the operation
 */
gboolean init(GtkWidget* widget, gpointer data);

//! Function to force a redraw of the widget
void redraw(GtkWidget* widget);

//! Function that creates the opengl widget with all the proper information
/*!
 * \return the created widget
 */
GtkWidget* createGLWidget();

//! Draws the images from the local capture source
/*!
 * \param widget a pointer to the widget being drawned
 * \param data data on the call back
 * \param glContext the context of the glwidget
 * \param glDrawable the drawing info of the glwidget
 * \return the success of the operation
 */
gboolean drawLocal(GtkWidget* widget, gpointer data, GdkGLContext *glContext, GdkGLDrawable *glDrawable);


//! Draws the images from the remote source
/*!
 * \param widget a pointer to the widget being drawned
 * \param data data on the call back
 * \param glContext the context of the glwidget
 * \param glDrawable the drawing info of the glwidget
 * \return the success of the operation
 */
gboolean drawRemote(GtkWidget* widget, gpointer data, GdkGLContext *glContext, GdkGLDrawable *glDrawable);

/**
 * Intitializes the shred memory spaces
 * @param local A pointer to the local key in a caracter format
 * @param remote A pointer to the remote key in a caracter format
 */
gboolean InitMemSpaces( char* local, char* remote );

/**
 * Destroys the memspaces
 */
gboolean DestroyMemSpaces();

#endif //GLWIDGET_H
