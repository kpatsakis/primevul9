static void mpgviddmx_finalize(GF_Filter *filter)
{
	GF_MPGVidDmxCtx *ctx = gf_filter_get_udta(filter);
	if (ctx->bs) gf_bs_del(ctx->bs);
	if (ctx->vparser) gf_m4v_parser_del_no_bs(ctx->vparser);
	if (ctx->indexes) gf_free(ctx->indexes);
	if (ctx->hdr_store) gf_free(ctx->hdr_store);
	if (ctx->pck_queue) {
		while (gf_list_count(ctx->pck_queue)) {
			GF_FilterPacket *pck = gf_list_pop_back(ctx->pck_queue);
			gf_filter_pck_discard(pck);
		}
		gf_list_del(ctx->pck_queue);
	}
	if (ctx->src_pck) gf_filter_pck_unref(ctx->src_pck);
	if (ctx->importer) {
		GF_LOG(GF_LOG_INFO, GF_LOG_AUTHOR, ("%s Import results: %d VOPs (%d Is - %d Ps - %d Bs)\n", ctx->is_mpg12 ? "MPEG-1/2" : "MPEG-4 (Part 2)", ctx->nb_frames, ctx->nb_i, ctx->nb_p, ctx->nb_b));
		if (ctx->nb_b) {
			GF_LOG(GF_LOG_INFO, GF_LOG_AUTHOR, ("\t%d max consecutive B-frames%s\n", ctx->max_b, ctx->is_packed ? " - packed bitstream" : "" ));
		}
		if (ctx->is_vfr && ctx->nb_b && ctx->is_packed) {
			GF_LOG(GF_LOG_INFO, GF_LOG_AUTHOR, ("Warning: Mix of non-coded frames: packed bitstream and encoder skiped - unpredictable timing\n"));
		}
	}
}