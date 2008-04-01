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

#include <glwidget.h>
#include <math.h>

float LocalZoom= 0.05;

gboolean draw(GtkWidget* widget, gpointer data)
{
	GdkGLContext *glcontext = gtk_widget_get_gl_context (widget);
  	GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (widget);

    // OpenGL BEGIN
  	if (!gdk_gl_drawable_gl_begin (gldrawable, glcontext)){
    	return FALSE;
  	}
	
	// Viewport definition
	glViewport (0, 0, widget->allocation.width, widget->allocation.height);
		    
	// Loading Initial drawing info 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, widget->allocation.width, 0.0, widget->allocation.height );
	
	// Draw Recevied Images
	if( prefPannel != TRUE ){
		
		drawRemote(widget, data, glcontext, gldrawable);
		
		// ReLoading Initial drawing info
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0.0, widget->allocation.width, 0.0, widget->allocation.height );
		
	}
	
	// Draw Local Image	
	drawLocal(widget, data, glcontext, gldrawable);
		
	if (gdk_gl_drawable_is_double_buffered (gldrawable))
   	 	gdk_gl_drawable_swap_buffers (gldrawable);
 	else
    	glFlush ();
    	
    // OpenGL END
    gdk_gl_drawable_gl_end (gldrawable);
	
	// Setup redraw
	redraw(widget);
	
	return TRUE;
	
}

gboolean reshape(GtkWidget* widget, GdkEventConfigure* ev, gpointer data)
{
	
	glViewport (0, 0, widget->allocation.width, widget->allocation.height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, widget->allocation.width, 0.0, widget->allocation.height );
	return TRUE;
}

gboolean init(GtkWidget* widget, gpointer data)
{
	GdkGLContext *glcontext = gtk_widget_get_gl_context (widget);
  	GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (widget);
 
 	// OpenGl BEGIN 	
    if (!gdk_gl_drawable_gl_begin (gldrawable, glcontext))
    	return FALSE;

    glViewport (0, 0,widget->allocation.width, widget->allocation.height);
              
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	// Opengl END
	gdk_gl_drawable_gl_end (gldrawable);

    redraw(widget);
	return TRUE;
}

void redraw(GtkWidget* widget)
{
	gtk_widget_queue_draw(GTK_WIDGET(widget));
}

GtkWidget* createGLWidget( gboolean pref )
{	
	
	 prefPannel= pref;
	
	 GtkWidget* widget;
	
  	 g_print("Info: gtkglext Initialisation\n");
 	 gtk_gl_init(NULL,NULL);
 	 glEnable(GL_DEPTH_TEST);
  
  	// Try double-buffered configuration
  	glconfig = gdk_gl_config_new_by_mode(GDK_GL_MODE_RGB    |
                                        GDK_GL_MODE_DEPTH  |
                                        GDK_GL_MODE_DOUBLE);
  	if (glconfig == NULL)
    {
      	g_print ("Warning: Cannot find double-buffered visual.\n");
      	g_print ("Info: Trying single-buffered visual.\n");

      	// Try single-buffered configuration
      	glconfig = gdk_gl_config_new_by_mode( GDK_GL_MODE_RGB   |
      										  GDK_GL_MODE_DEPTH);
      	if (glconfig == NULL)
        {
          	g_print ("Error: No appropriate OpenGL-capable visual found.\n");
          	return NULL;
        }
    }

	g_print("Info: Initialisation of the drawing space\n");
	widget = (GtkWidget*)gtk_drawing_area_new ();

	// TODO: Check dynamic size not fixed, does it make a difference
	gtk_widget_set_size_request (widget, 300, 300);
		      
  	gtk_widget_set_gl_capability (widget, glconfig, NULL, TRUE, GDK_GL_RGBA_TYPE);
  	
  	// Callbacks connections
	g_signal_connect(G_OBJECT(widget), "realize", G_CALLBACK(init), NULL);
	g_signal_connect(G_OBJECT(widget), "configure_event", G_CALLBACK(reshape), NULL);
	g_signal_connect(G_OBJECT(widget), "expose_event", G_CALLBACK(draw), NULL);
	
	return widget;

}

gboolean drawLocal(GtkWidget* widget, gpointer data, GdkGLContext *glContext, GdkGLDrawable *glDrawable)
{
	
	// Fetch Data in the local memory space
	if( fetchData(localKey, localBuff) < 0 ){
		return FALSE;
	}
			
	if( localBuff->size == 0){
		return FALSE;
	}
	
	float zoomFactor= 0.0, zoomX= 1.0, zoomY= -1.0, x= 0.0, y= 0.0;
	
	if( prefPannel == TRUE ){
		
		if( currentGLWidth != localBuff->width + 10 || currentGLHeight != localBuff->height + 10 ){
			currentGLWidth= localBuff->width + 10;
			currentGLHeight= localBuff->height + 10;
			gtk_widget_set_size_request (widget, currentGLWidth, currentGLHeight);
			return FALSE;
		}
		
		x= ( widget->allocation.width - localBuff->width)/2.0;
		
		if( widget->allocation.height == localBuff->height  )
			y= ( widget->allocation.height - localBuff->height)/2.0;
		else
			y= ( widget->allocation.height - localBuff->height)/2.0  + (float)localBuff->height;
	
	}else{
	
		float localArea = 0, allocationArea = 0;
		
		localArea = (float)(localBuff->width*localBuff->height);
		allocationArea = (float)(widget->allocation.height*widget->allocation.width*LocalZoom);
		
		// Calculating zoom Factor for local display
		zoomFactor = sqrt( allocationArea / localArea );
		
		zoomX= zoomX * zoomFactor;
		zoomY= zoomY * zoomFactor;
			
		// Calculating raster position (Drawing position)
		x= widget->allocation.width - ((float)localBuff->width * zoomX) - 20;
		y= ((float)localBuff->height * zoomX  ) + 20;
		
	
	}
	
	// Draw fetched Data
	glClear(GL_COLOR_BUFFER_BIT);
	glPixelStorei(GL_PACK_ALIGNMENT, 8);	
	glRasterPos2f(x,y);
	glPixelZoom( zoomX, zoomY );
	glDrawPixels(localBuff->width, localBuff->height, GL_RGB, GL_UNSIGNED_BYTE, localBuff->data );
		
	return TRUE;
	
}

gboolean drawRemote(GtkWidget* widget, gpointer data, GdkGLContext *glContext, GdkGLDrawable *glDrawable)
{
	
	// Fetch Data in the remote memory space
	if( fetchData(remoteKey, remoteBuff) < 0 ){
		return FALSE;
	}
	
	if( remoteBuff->data == 0){
		return FALSE;
	}
	
	if( currentGLWidth != remoteBuff->width + 10 || currentGLHeight != remoteBuff->height + 10 ){
		currentGLWidth= remoteBuff->width + 10;
		currentGLHeight= remoteBuff->height + 10;
		gtk_widget_set_size_request (widget, currentGLWidth, currentGLHeight);
		return FALSE;
	}
	
	// Draw fetched Data
	glClear(GL_COLOR_BUFFER_BIT);		
	glPixelStorei(GL_PACK_ALIGNMENT, 8);
	
	// Calculating raster position (Drawing position), centers the image
	float x= ( widget->allocation.width - remoteBuff->width)/2.0;
	float y= 0.0;
	if( widget->allocation.height == remoteBuff->height  )
		y= ( widget->allocation.height - remoteBuff->height)/2.0;
	else
		y= ( widget->allocation.height - remoteBuff->height)/2.0  + (float)remoteBuff->height;

	glRasterPos2f(x,y);
	
	
	glPixelZoom(1., -1.);
	glDrawPixels(remoteBuff->width, remoteBuff->height, GL_RGB, GL_UNSIGNED_BYTE, remoteBuff->data );
	
	return TRUE;
	
}
