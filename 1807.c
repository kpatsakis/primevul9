static Bool mpgviddmx_process_event(GF_Filter *filter, const GF_FilterEvent *evt)
{
	u32 i;
	u64 file_pos = 0;
	GF_FilterEvent fevt;
	GF_MPGVidDmxCtx *ctx = gf_filter_get_udta(filter);

	switch (evt->base.type) {
	case GF_FEVT_PLAY:
		if (!ctx->is_playing) {
			ctx->is_playing = GF_TRUE;
			ctx->cts = 0;
			ctx->bytes_in_header = 0;
		}
		if (! ctx->is_file) {
			if (!ctx->initial_play_done) {
				ctx->initial_play_done = GF_TRUE;
				if (evt->play.start_range>0.1) ctx->resume_from = 0;
 			}
			return GF_FALSE;
		}
		ctx->start_range = evt->play.start_range;
		ctx->in_seek = GF_TRUE;

		if (ctx->start_range) {
			for (i=1; i<ctx->index_size; i++) {
				if ((ctx->indexes[i].start_time > ctx->start_range) || (i+1==ctx->index_size)) {
					ctx->cts = (u64) (ctx->indexes[i-1].start_time * ctx->cur_fps.num);
					file_pos = ctx->indexes[i-1].pos;
					break;
				}
			}
		}
		ctx->dts = ctx->cts;
		if (!ctx->initial_play_done) {
			ctx->initial_play_done = GF_TRUE;
			//seek will not change the current source state, don't send a seek
			if (!file_pos)
				return GF_TRUE;
		}
		ctx->resume_from = 0;
		ctx->bytes_in_header = 0;
		//post a seek
		GF_FEVT_INIT(fevt, GF_FEVT_SOURCE_SEEK, ctx->ipid);
		fevt.seek.start_offset = file_pos;
		gf_filter_pid_send_event(ctx->ipid, &fevt);

		//cancel event
		return GF_TRUE;

	case GF_FEVT_STOP:
		ctx->is_playing = GF_FALSE;
		if (ctx->src_pck) gf_filter_pck_unref(ctx->src_pck);
		ctx->src_pck = NULL;
		if (ctx->pck_queue) {
			while (gf_list_count(ctx->pck_queue)) {
				GF_FilterPacket *pck=gf_list_pop_front(ctx->pck_queue);
				gf_filter_pck_discard(pck);
			}
		}
		//don't cancel event
		return GF_FALSE;

	case GF_FEVT_SET_SPEED:
		//cancel event
		return GF_TRUE;
	default:
		break;
	}
	//by default don't cancel event - to rework once we have downloading in place
	return GF_FALSE;
}