komeda_wb_encoder_atomic_check(struct drm_encoder *encoder,
			       struct drm_crtc_state *crtc_st,
			       struct drm_connector_state *conn_st)
{
	struct komeda_crtc_state *kcrtc_st = to_kcrtc_st(crtc_st);
	struct drm_writeback_job *writeback_job = conn_st->writeback_job;
	struct komeda_layer *wb_layer;
	struct komeda_data_flow_cfg dflow;
	int err;

	if (!writeback_job)
		return 0;

	if (!crtc_st->active) {
		DRM_DEBUG_ATOMIC("Cannot write the composition result out on a inactive CRTC.\n");
		return -EINVAL;
	}

	wb_layer = to_kconn(to_wb_conn(conn_st->connector))->wb_layer;

	/*
	 * No need for a full modested when the only connector changed is the
	 * writeback connector.
	 */
	if (crtc_st->connectors_changed &&
	    is_only_changed_connector(crtc_st, conn_st->connector))
		crtc_st->connectors_changed = false;

	err = komeda_wb_init_data_flow(wb_layer, conn_st, kcrtc_st, &dflow);
	if (err)
		return err;

	if (dflow.en_split)
		err = komeda_build_wb_split_data_flow(wb_layer,
				conn_st, kcrtc_st, &dflow);
	else
		err = komeda_build_wb_data_flow(wb_layer,
				conn_st, kcrtc_st, &dflow);

	return err;
}