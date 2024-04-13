dvi_document_finalize (GObject *object)
{	
	DviDocument *dvi_document = DVI_DOCUMENT(object);
	
	g_mutex_lock (&dvi_context_mutex);
	if (dvi_document->context) {
		mdvi_cairo_device_free (&dvi_document->context->device);
		mdvi_destroy_context (dvi_document->context);
	}
	g_mutex_unlock (&dvi_context_mutex);

	if (dvi_document->params)
		g_free (dvi_document->params);

	if (dvi_document->exporter_filename)
		g_free (dvi_document->exporter_filename);
	
	if (dvi_document->exporter_opts)
		g_string_free (dvi_document->exporter_opts, TRUE);

        g_free (dvi_document->uri);
		
	G_OBJECT_CLASS (dvi_document_parent_class)->finalize (object);
}