dvi_document_load (EvDocument  *document,
		   const char  *uri,
		   GError     **error)
{
	gchar *filename;
	DviDocument *dvi_document = DVI_DOCUMENT(document);
	
	filename = g_filename_from_uri (uri, NULL, error);
	if (!filename)
        	return FALSE;
	
	g_mutex_lock (&dvi_context_mutex);
	if (dvi_document->context)
		mdvi_destroy_context (dvi_document->context);

	dvi_document->context = mdvi_init_context(dvi_document->params, dvi_document->spec, filename);
	g_mutex_unlock (&dvi_context_mutex);
	g_free (filename);
	
	if (!dvi_document->context) {
    		g_set_error_literal (error,
                                     EV_DOCUMENT_ERROR,
                                     EV_DOCUMENT_ERROR_INVALID,
                                     _("DVI document has incorrect format"));
        	return FALSE;
	}
	
	mdvi_cairo_device_init (&dvi_document->context->device);
	
	
	dvi_document->base_width = dvi_document->context->dvi_page_w * dvi_document->context->params.conv 
		+ 2 * unit2pix(dvi_document->params->dpi, MDVI_HMARGIN) / dvi_document->params->hshrink;
	
	dvi_document->base_height = dvi_document->context->dvi_page_h * dvi_document->context->params.vconv 
	        + 2 * unit2pix(dvi_document->params->vdpi, MDVI_VMARGIN) / dvi_document->params->vshrink;
	
	g_free (dvi_document->uri);
	dvi_document->uri = g_strdup (uri);
	
	return TRUE;
}