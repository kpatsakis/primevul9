komeda_wb_init_data_flow(struct komeda_layer *wb_layer,
			 struct drm_connector_state *conn_st,
			 struct komeda_crtc_state *kcrtc_st,
			 struct komeda_data_flow_cfg *dflow)
{
	struct drm_framebuffer *fb = conn_st->writeback_job->fb;

	memset(dflow, 0, sizeof(*dflow));

	dflow->out_w = fb->width;
	dflow->out_h = fb->height;

	/* the write back data comes from the compiz */
	pipeline_composition_size(kcrtc_st, &dflow->in_w, &dflow->in_h);
	dflow->input.component = &wb_layer->base.pipeline->compiz->base;
	/* compiz doesn't output alpha */
	dflow->pixel_blend_mode = DRM_MODE_BLEND_PIXEL_NONE;
	dflow->rot = DRM_MODE_ROTATE_0;

	komeda_complete_data_flow_cfg(wb_layer, dflow, fb);

	return 0;
}