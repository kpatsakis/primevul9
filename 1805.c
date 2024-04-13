static void mpgviddmx_check_pid(GF_Filter *filter, GF_MPGVidDmxCtx *ctx, u32 vosh_size, u8 *data)
{
	if (!ctx->opid) {
		ctx->opid = gf_filter_pid_new(filter);
		gf_filter_pid_copy_properties(ctx->opid, ctx->ipid);
		mpgviddmx_check_dur(filter, ctx);
	}

	if ((ctx->width == ctx->dsi.width) && (ctx->height == ctx->dsi.height)) return;

	//copy properties at init or reconfig
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_STREAM_TYPE, & PROP_UINT(GF_STREAM_VISUAL));
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_TIMESCALE, & PROP_UINT(ctx->timescale ? ctx->timescale : ctx->cur_fps.num));
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_FPS, & PROP_FRAC(ctx->cur_fps));
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_UNFRAMED, NULL);
	if (ctx->duration.num)
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_DURATION, & PROP_FRAC64(ctx->duration));

	mpgviddmx_enqueue_or_dispatch(ctx, NULL, GF_TRUE, GF_FALSE);

	ctx->width = ctx->dsi.width;
	ctx->height = ctx->dsi.height;
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_WIDTH, & PROP_UINT( ctx->dsi.width));
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_HEIGHT, & PROP_UINT( ctx->dsi.height));
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_SAR, & PROP_FRAC_INT(ctx->dsi.par_num, ctx->dsi.par_den));

	if (ctx->is_mpg12) {
		const GF_PropertyValue *cid = gf_filter_pid_get_property(ctx->ipid, GF_PROP_PID_CODECID);
		u32 PL = ctx->dsi.VideoPL;
		if (cid) {
			switch (cid->value.uint) {
			case GF_CODECID_MPEG2_MAIN:
			case GF_CODECID_MPEG2_422:
			case GF_CODECID_MPEG2_SNR:
			case GF_CODECID_MPEG2_HIGH:
				//keep same signaling
				PL = cid->value.uint;
				break;
			default:
				break;
			}
		}
		if (!PL) PL = GF_CODECID_MPEG2_MAIN;
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_CODECID, & PROP_UINT(PL));
	} else {
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_CODECID, & PROP_UINT(GF_CODECID_MPEG4_PART2));
	}
	gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_PROFILE_LEVEL, & PROP_UINT (ctx->dsi.VideoPL) );

	if (ctx->bitrate) {
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_BITRATE, & PROP_UINT(ctx->bitrate));
	}

	ctx->b_frames = 0;

	if (vosh_size) {
		u32 i;
		char * dcfg = gf_malloc(sizeof(char)*vosh_size);
		memcpy(dcfg, data, sizeof(char)*vosh_size);

		/*remove packed flag if any (VOSH user data)*/
		ctx->is_packed = ctx->is_vfr = ctx->forced_packed = GF_FALSE;
		i=0;
		while (1) {
			char *frame = dcfg;
			while ((i+3<vosh_size)  && ((frame[i]!=0) || (frame[i+1]!=0) || (frame[i+2]!=1))) i++;
			if (i+4>=vosh_size) break;
			if (strncmp(frame+i+4, "DivX", 4)) {
				i += 4;
				continue;
			}
			frame = memchr(dcfg + i + 4, 'p', vosh_size - i - 4);
			if (frame) {
				ctx->forced_packed = GF_TRUE;
				frame[0] = 'n';
			}
			break;
		}
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_DECODER_CONFIG, & PROP_DATA_NO_COPY(dcfg, vosh_size));
	}

	if (ctx->is_file && ctx->index) {
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_PLAYBACK_MODE, & PROP_UINT(GF_PLAYBACK_MODE_FASTFORWARD) );
	}
}