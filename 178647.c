gdk_pixbuf__jpeg_image_begin_load (GdkPixbufModuleSizeFunc size_func,
				   GdkPixbufModulePreparedFunc prepared_func, 
				   GdkPixbufModuleUpdatedFunc updated_func,
				   gpointer user_data,
                                   GError **error)
{
	JpegProgContext *context;
	my_source_mgr   *src;

	context = g_new0 (JpegProgContext, 1);
	context->size_func = size_func;
	context->prepared_func = prepared_func;
	context->updated_func  = updated_func;
	context->user_data = user_data;
	context->pixbuf = NULL;
	context->got_header = FALSE;
	context->did_prescan = FALSE;
	context->src_initialized = FALSE;
	context->in_output = FALSE;

        /* From jpeglib.h: "NB: you must set up the error-manager
         * BEFORE calling jpeg_create_xxx". */
	context->cinfo.err = jpeg_std_error (&context->jerr.pub);
	context->jerr.pub.error_exit = fatal_error_handler;
        context->jerr.pub.output_message = output_message_handler;
        context->jerr.error = error;

        if (sigsetjmp (context->jerr.setjmp_buffer, 1)) {
                jpeg_destroy_decompress (&context->cinfo);
                g_free(context);
                /* error should have been set by fatal_error_handler () */
                return NULL;
        }

	/* create libjpeg structures */
	jpeg_create_decompress (&context->cinfo);

	context->cinfo.src = (struct jpeg_source_mgr *) g_try_malloc (sizeof (my_source_mgr));
	if (!context->cinfo.src) {
		g_set_error_literal (error,
                                     GDK_PIXBUF_ERROR,
                                     GDK_PIXBUF_ERROR_INSUFFICIENT_MEMORY,
                                     _("Couldn't allocate memory for loading JPEG file"));
		return NULL;
	}
	memset (context->cinfo.src, 0, sizeof (my_source_mgr));

	src = (my_src_ptr) context->cinfo.src;
	src->pub.init_source = init_source;
	src->pub.fill_input_buffer = fill_input_buffer;
	src->pub.skip_input_data = skip_input_data;
	src->pub.resync_to_restart = jpeg_resync_to_restart;
	src->pub.term_source = term_source;
	src->pub.bytes_in_buffer = 0;
	src->pub.next_input_byte = NULL;

        context->jerr.error = NULL;
        
	return (gpointer) context;
}