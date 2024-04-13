dvi_document_render (EvDocument      *document,
		     EvRenderContext *rc)
{
	cairo_surface_t *surface;
	cairo_surface_t *rotated_surface;
	DviDocument *dvi_document = DVI_DOCUMENT(document);
	gdouble xscale, yscale;
	gint required_width, required_height;
	gint proposed_width, proposed_height;
	gint xmargin = 0, ymargin = 0;

	/* We should protect our context since it's not 
	 * thread safe. The work to the future - 
	 * let context render page independently
	 */
	g_mutex_lock (&dvi_context_mutex);
	
	mdvi_setpage (dvi_document->context, rc->page->index);
	
	ev_render_context_compute_scales (rc, dvi_document->base_width, dvi_document->base_height,
					  &xscale, &yscale);
	mdvi_set_shrink (dvi_document->context, 
			 (int)((dvi_document->params->hshrink - 1) / xscale) + 1,
			 (int)((dvi_document->params->vshrink - 1) / yscale) + 1);

	ev_render_context_compute_scaled_size (rc, dvi_document->base_width, dvi_document->base_height,
					       &required_width, &required_height);
	proposed_width = dvi_document->context->dvi_page_w * dvi_document->context->params.conv;
	proposed_height = dvi_document->context->dvi_page_h * dvi_document->context->params.vconv;
	
	if (required_width >= proposed_width)
	    xmargin = (required_width - proposed_width) / 2;
	if (required_height >= proposed_height)
	    ymargin = (required_height - proposed_height) / 2;
	    
	mdvi_cairo_device_set_margins (&dvi_document->context->device, xmargin, ymargin);
	mdvi_cairo_device_set_scale (&dvi_document->context->device, xscale, yscale);
	mdvi_cairo_device_render (dvi_document->context);
	surface = mdvi_cairo_device_get_surface (&dvi_document->context->device);

	g_mutex_unlock (&dvi_context_mutex);

	rotated_surface = ev_document_misc_surface_rotate_and_scale (surface,
								     required_width,
								     required_height, 
								     rc->rotation);
	cairo_surface_destroy (surface);
	
	return rotated_surface;
}