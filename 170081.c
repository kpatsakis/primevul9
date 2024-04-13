gdk_pixbuf__bmp_image_begin_load(GdkPixbufModuleSizeFunc size_func,
                                 GdkPixbufModulePreparedFunc prepared_func,
				 GdkPixbufModuleUpdatedFunc updated_func,
                                 gpointer user_data,
                                 GError **error)
{
	struct bmp_progressive_state *context;
	
	context = g_new0(struct bmp_progressive_state, 1);
	context->size_func = size_func;
	context->prepared_func = prepared_func;
	context->updated_func = updated_func;
	context->user_data = user_data;

	context->read_state = READ_STATE_HEADERS;

	context->BufferSize = 26;
	context->BufferPadding = 0;
	context->buff = g_malloc(26);
	context->BufferDone = 0;
	/* 14 for the BitmapFileHeader, 12 for the BitmapImageHeader */

	context->Colormap = NULL;

	context->Lines = 0;

	context->Type = 0;

	memset(&context->Header, 0, sizeof(struct headerpair));
	memset(&context->compr, 0, sizeof(struct bmp_compression_state));


	context->pixbuf = NULL;
	
	return (gpointer) context;
}