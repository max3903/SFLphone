#include <glwidget.h>

//! Callback Function to draw the content of the widget
/*!
 * \param widget a pointer to the widget being drawned
 * \param data data on the call back
 * \return the success of the operation
 */
gboolean draw(GtkWidget* widget, gpointer data)
{
	GdkGLContext *glcontext = gtk_widget_get_gl_context (widget);
  	GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (widget);

    // OpenGL BEGIN
  	if (!gdk_gl_drawable_gl_begin (gldrawable, glcontext))
    	return FALSE;
		
	// Viewport definition
	glViewport (0, 0, widget->allocation.width, widget->allocation.height);
	    
	// Loading Initial drawing info 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	// Draw Recevied Images
	drawRemote(widget, data, glcontext, gldrawable);
	
	// ReLoading Initial drawing info
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
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

//! Callback Function to reshape the content of the widget
/*!
 * \param widget a pointer to the widget being drawned
 * \param ev a pointer to the event data
 * \param data data on the call back
 * \return the success of the operation
 */
gboolean reshape(GtkWidget* widget, GdkEventConfigure* ev, gpointer data)
{
	
	// TODO: Reshape pour conserer le rapport d'aspect
	return TRUE;
}

//! Callback Function to initialise the content of the widget
/*!
 * \param widget a pointer to the widget being drawned
 * \param data data on the call back
 * \return the success of the operation
 */
gboolean init(GtkWidget* widget, gpointer data)
{
	GdkGLContext *glcontext = gtk_widget_get_gl_context (widget);
  	GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable (widget);
 
 	// OpenGl BEGIN 	
    if (!gdk_gl_drawable_gl_begin (gldrawable, glcontext))
    	return;

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

//! Function to force a redraw of the widget
void redraw(GtkWidget* widget)
{
	gtk_widget_queue_draw(GTK_WIDGET(widget));
}

//! Function that creates the opengl widget with all the proper information
/*!
 * \return the created widget
 */
GtkWidget* createGLWidget()
{	
	
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
	drawing_area = (GtkWidget*)gtk_drawing_area_new ();

	// TODO: Check dynamic size not fixed, does it make a difference
	gtk_widget_set_size_request (drawing_area, 300, 300);
		      
  	gtk_widget_set_gl_capability (drawing_area, glconfig, NULL, TRUE, GDK_GL_RGBA_TYPE);
  	
  	// Callbacks connections
	g_signal_connect(G_OBJECT(drawing_area), "realize", G_CALLBACK(init), NULL);
	g_signal_connect(G_OBJECT(drawing_area), "configure_event", G_CALLBACK(reshape), NULL);
	g_signal_connect(G_OBJECT(drawing_area), "expose_event", G_CALLBACK(draw), NULL);
	
	return drawing_area;

}

//! Draws the images from the local capture source
/*!
 * \param widget a pointer to the widget being drawned
 * \param data data on the call back
 * \return the success of the operation
 */
gboolean drawLocal(GtkWidget* widget, gpointer data, GdkGLContext *glContext, GdkGLDrawable *glDrawable)
{
	
	// Fetch Data in the local memory space
	//TODO: Add MemManager code to fetch data
	
	// Draw fetched Data
	/*glClear(GL_COLOR_BUFFER_BIT);		
	glRasterPos2f(-1.0*(widget->allocation.width/2.0+camera->width/2.0)/1000.0,(widget->allocation.height/2.0+camera->height*3)/1000.0);
	glPixelStorei(GL_PACK_ALIGNMENT, 8);
	glPixelZoom(1., -1.);
	glDrawPixels(camera->width, camera->height, GL_RGB, GL_UNSIGNED_BYTE, pix );*/
	return TRUE;
}


//! Draws the images from the remote source
/*!
 * \param widget a pointer to the widget being drawned
 * \param data data on the call back
 * \return the success of the operation
 */
gboolean drawRemote(GtkWidget* widget, gpointer data, GdkGLContext *glContext, GdkGLDrawable *glDrawable)
{
	
	// Fetch Data in the remote memory space
	//TODO: Add MemManager code to fetch data
	
	// Draw fetched Data
	/*glClear(GL_COLOR_BUFFER_BIT);		
	glRasterPos2f(-1.0*(widget->allocation.width/2.0+camera->width/2.0)/1000.0,(widget->allocation.height/2.0+camera->height*3)/1000.0);
	glPixelStorei(GL_PACK_ALIGNMENT, 8);
	glPixelZoom(1., -1.);
	glDrawPixels(camera->width, camera->height, GL_RGB, GL_UNSIGNED_BYTE, pix );*/
	return TRUE;
}
