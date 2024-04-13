gdk_pixbuf__gif_image_begin_load (GdkPixbufModuleSizeFunc size_func,
                                  GdkPixbufModulePreparedFunc prepare_func,
				  GdkPixbufModuleUpdatedFunc update_func,
				  gpointer user_data,
                                  GError **error)
{
	GifContext *context;

#ifdef IO_GIFDEBUG
	count = 0;
#endif
	context = new_context ();

        if (context == NULL) {
                g_set_error_literal (error,
                                     GDK_PIXBUF_ERROR,
                                     GDK_PIXBUF_ERROR_INSUFFICIENT_MEMORY,
                                     _("Not enough memory to load GIF file"));
                return NULL;
        }
        
        context->error = error;
	context->prepare_func = prepare_func;
	context->update_func = update_func;
	context->user_data = user_data;

	return (gpointer) context;
}