static void mpgviddmx_check_dur(GF_Filter *filter, GF_MPGVidDmxCtx *ctx)
{

	FILE *stream;
	GF_BitStream *bs;
	GF_M4VParser *vparser;
	GF_M4VDecSpecInfo dsi;
	GF_Err e;
	u64 duration, cur_dur, rate;
	const GF_PropertyValue *p;
	if (!ctx->opid || ctx->timescale || ctx->file_loaded) return;

	if (ctx->index<=0) {
		ctx->file_loaded = GF_TRUE;
		return;
	}

	p = gf_filter_pid_get_property(ctx->ipid, GF_PROP_PID_FILEPATH);
	if (!p || !p->value.string || !strncmp(p->value.string, "gmem://", 7)) {
		ctx->is_file = GF_FALSE;
		ctx->file_loaded = GF_TRUE;
		return;
	}
	ctx->is_file = GF_TRUE;

	stream = gf_fopen(p->value.string, "rb");
	if (!stream) return;

	ctx->index_size = 0;

	bs = gf_bs_from_file(stream, GF_BITSTREAM_READ);

	vparser = gf_m4v_parser_bs_new(bs, ctx->is_mpg12);
	e = gf_m4v_parse_config(vparser, &dsi);
	if (e) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_MEDIA, ("[MPGVid] Could not parse video header - duration  not estimated\n"));
		ctx->file_loaded = GF_TRUE;
		return;
	}

	duration = 0;
	cur_dur = 0;
	while (gf_bs_available(bs)) {
		u8 ftype;
		u32 tinc;
		u64 fsize, start;
		Bool is_coded;
		u64 pos;
		pos = gf_m4v_get_object_start(vparser);
		e = gf_m4v_parse_frame(vparser, &dsi, &ftype, &tinc, &fsize, &start, &is_coded);
		if (e<0) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_MEDIA, ("[MPGVid] Could not parse video frame\n"));
			continue;
		}

		duration += ctx->cur_fps.den;
		cur_dur += ctx->cur_fps.den;
		//only index at I-frame start
		if (pos && (ftype==0) && (cur_dur >= ctx->index * ctx->cur_fps.num) ) {
			if (!ctx->index_alloc_size) ctx->index_alloc_size = 10;
			else if (ctx->index_alloc_size == ctx->index_size) ctx->index_alloc_size *= 2;
			ctx->indexes = gf_realloc(ctx->indexes, sizeof(MPGVidIdx)*ctx->index_alloc_size);
			ctx->indexes[ctx->index_size].pos = pos;
			ctx->indexes[ctx->index_size].start_time = (Double) (duration-ctx->cur_fps.den);
			ctx->indexes[ctx->index_size].start_time /= ctx->cur_fps.num;
			ctx->index_size ++;
			cur_dur = 0;
		}
	}
	rate = gf_bs_get_position(bs);
	gf_m4v_parser_del(vparser);
	gf_fclose(stream);

	if (!ctx->duration.num || (ctx->duration.num  * ctx->cur_fps.num != duration * ctx->duration.den)) {
		ctx->duration.num = (s32) duration;
		ctx->duration.den = ctx->cur_fps.num;

		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_DURATION, & PROP_FRAC64(ctx->duration));

		if (duration && !gf_sys_is_test_mode() ) {
			rate *= 8 * ctx->duration.den;
			rate /= ctx->duration.num;
			ctx->bitrate = (u32) rate;
		}
	}

	p = gf_filter_pid_get_property(ctx->ipid, GF_PROP_PID_FILE_CACHED);
	if (p && p->value.boolean) ctx->file_loaded = GF_TRUE;
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_CAN_DATAREF, & PROP_BOOL(GF_TRUE ) );
}