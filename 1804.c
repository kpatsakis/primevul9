static void mpgviddmx_enqueue_or_dispatch(GF_MPGVidDmxCtx *ctx, GF_FilterPacket *pck, Bool flush_ref, Bool is_eos)
{
	//TODO: we are dispatching frames in "negctts mode", ie we may have DTS>CTS
	//need to signal this for consumers using DTS (eg MPEG-2 TS)
	if (flush_ref && ctx->pck_queue) {
		//send all reference packet queued
		u32 i, count = gf_list_count(ctx->pck_queue);

		for (i=0; i<count; i++) {
			u64 cts;
			GF_FilterPacket *q_pck = gf_list_get(ctx->pck_queue, i);
			u8 carousel = gf_filter_pck_get_carousel_version(q_pck);
			if (!carousel) {
				gf_filter_pck_send(q_pck);
				continue;
			}
			gf_filter_pck_set_carousel_version(q_pck, 0);
			cts = gf_filter_pck_get_cts(q_pck);
			if (cts != GF_FILTER_NO_TS) {
				//offset the cts of the ref frame to the number of B frames in-between
				if (ctx->last_ref_cts == cts) {
					cts += ctx->b_frames * ctx->cur_fps.den;
					gf_filter_pck_set_cts(q_pck, cts);
				} else {
					//shift all other frames (i.e. pending Bs) by 1 frame in the past since we move the ref frame after them
					assert(cts >= ctx->cur_fps.den);
					cts -= ctx->cur_fps.den;
					gf_filter_pck_set_cts(q_pck, cts);
				}
			}
			if (is_eos && (i+1==count)) {
				Bool start, end;
				gf_filter_pck_get_framing(q_pck, &start, &end);
				gf_filter_pck_set_framing(q_pck, start, GF_TRUE);
			}
			gf_filter_pck_send(q_pck);
		}
		gf_list_reset(ctx->pck_queue);
	}
	if (!pck) return;

	if (!ctx->pck_queue) ctx->pck_queue = gf_list_new();
	gf_list_add(ctx->pck_queue, pck);
}