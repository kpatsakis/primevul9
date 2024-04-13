gdk_pixbuf__tiff_image_begin_load (GdkPixbufModuleSizeFunc size_func,
                                   GdkPixbufModulePreparedFunc prepare_func,
				   GdkPixbufModuleUpdatedFunc update_func,
				   gpointer user_data,
                                   GError **error)
{
	TiffContext *context;
        
	context = g_new0 (TiffContext, 1);
	context->size_func = size_func;
	context->prepare_func = prepare_func;
	context->update_func = update_func;
	context->user_data = user_data;
        context->buffer = NULL;
        context->allocated = 0;
        context->used = 0;
        context->pos = 0;
        
	return context;
}