GF_Err mpgviddmx_configure_pid(GF_Filter *filter, GF_FilterPid *pid, Bool is_remove)
{
	Bool was_mpeg12;
	const GF_PropertyValue *p;
	GF_MPGVidDmxCtx *ctx = gf_filter_get_udta(filter);

	if (is_remove) {
		ctx->ipid = NULL;
		if (ctx->opid) {
			gf_filter_pid_remove(ctx->opid);
			ctx->opid = NULL;
		}
		return GF_OK;
	}
	if (! gf_filter_pid_check_caps(pid))
		return GF_NOT_SUPPORTED;

	ctx->ipid = pid;
	ctx->cur_fps = ctx->fps;
	if (!ctx->fps.num || !ctx->fps.den) {
		ctx->cur_fps.num = 25000;
		ctx->cur_fps.den = 1000;
	}

	p = gf_filter_pid_get_property(pid, GF_PROP_PID_TIMESCALE);
	if (p) {
		ctx->timescale = ctx->cur_fps.num = p->value.uint;
		ctx->cur_fps.den = 0;
		p = gf_filter_pid_get_property(pid, GF_PROP_PID_FPS);
		if (p) {
			ctx->cur_fps = p->value.frac;
		}
		p = gf_filter_pid_get_property_str(pid, "nocts");
		if (p && p->value.boolean) ctx->recompute_cts = GF_TRUE;
	}

	was_mpeg12 = ctx->is_mpg12;

	p = gf_filter_pid_get_property(pid, GF_PROP_PID_CODECID);
	if (p) {
		switch (p->value.uint) {
		case GF_CODECID_MPEG1:
		case GF_CODECID_MPEG2_422:
		case GF_CODECID_MPEG2_SNR:
		case GF_CODECID_MPEG2_HIGH:
		case GF_CODECID_MPEG2_MAIN:
		case GF_CODECID_MPEG2_SIMPLE:
		case GF_CODECID_MPEG2_SPATIAL:
			ctx->is_mpg12 = GF_TRUE;
			break;
		}
	}
	else {
		p = gf_filter_pid_get_property(pid, GF_PROP_PID_MIME);
		if (p && p->value.string && (strstr(p->value.string, "m1v") || strstr(p->value.string, "m2v")) ) ctx->is_mpg12 = GF_TRUE;
		else {
			p = gf_filter_pid_get_property(pid, GF_PROP_PID_FILE_EXT);
			if (p && p->value.string && (strstr(p->value.string, "m1v") || strstr(p->value.string, "m2v")) ) ctx->is_mpg12 = GF_TRUE;
		}
	}

	if (ctx->vparser && (was_mpeg12 != ctx->is_mpg12)) {
		gf_m4v_parser_del_no_bs(ctx->vparser);
		ctx->vparser = NULL;
	}

	if (ctx->timescale && !ctx->opid) {
		ctx->opid = gf_filter_pid_new(filter);
		gf_filter_pid_copy_properties(ctx->opid, ctx->ipid);
		gf_filter_pid_set_property(ctx->opid, GF_PROP_PID_UNFRAMED, NULL);
	}
	return GF_OK;
}