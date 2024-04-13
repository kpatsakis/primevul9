static const char * mpgvdmx_probe_data(const u8 *data, u32 size, GF_FilterProbeScore *score)
{
	GF_M4VParser *parser;
	u8 ftype;
	u32 tinc, nb_frames, o_start=0;
	u64 fsize, start;
	Bool is_coded;
	GF_Err e;
	GF_M4VDecSpecInfo dsi;

	memset(&dsi, 0, sizeof(GF_M4VDecSpecInfo));
	parser = gf_m4v_parser_new((char*)data, size, GF_FALSE);
	nb_frames = 0;
	while (1) {
		u32 otype;
		ftype = 0;
		is_coded = GF_FALSE;
		e = gf_m4v_parse_frame(parser, &dsi, &ftype, &tinc, &fsize, &start, &is_coded);
		if (!nb_frames && start) o_start = (u32) start;

		otype = gf_m4v_parser_get_obj_type(parser);
		switch (otype) {
		case M4V_VOL_START_CODE:
		case M4V_VOP_START_CODE:
		case M4V_VISOBJ_START_CODE:
		case M4V_VOS_START_CODE:
		case M4V_GOV_START_CODE:
		case M4V_UDTA_START_CODE:
			break;
		default:
			otype = 0;
		}

		//if start is more than 4 (start-code size), we have garbage at the beginning, do not parse
		//except if we have a valid object VOS
		if (!nb_frames && (start>4) && !otype) {
			break;
		}
		if (is_coded) nb_frames++;
		if (e==GF_EOS) {
			e = GF_OK;
			//special case if the only frame we have is not coded
			if (otype == M4V_VOP_START_CODE) {
				if (!nb_frames) nb_frames++;
				is_coded = 1;
			}

			if (is_coded) nb_frames++;
			break;
		}
		if (ftype>2) break;
		if (e) break;
		nb_frames++;
	}
	gf_m4v_parser_del(parser);
	if ((e==GF_OK) && (nb_frames>1)) {
		*score = o_start ? GF_FPROBE_MAYBE_NOT_SUPPORTED : GF_FPROBE_MAYBE_SUPPORTED;
		return "video/mp4v-es";
	}

	memset(&dsi, 0, sizeof(GF_M4VDecSpecInfo));
	parser = gf_m4v_parser_new((char*)data, size, GF_TRUE);
	nb_frames = 0;
	while (1) {
		ftype = 0;
		is_coded = GF_FALSE;
		e = gf_m4v_parse_frame(parser, &dsi, &ftype, &tinc, &fsize, &start, &is_coded);

		//if start is more than 4 (start-code size), we have garbage at the beginning, do not parse
		if (!nb_frames && (start>4) ) {
			break;
		}
		if (is_coded) nb_frames++;
		if (e==GF_EOS) {
			if (is_coded) nb_frames++;
			e = GF_OK;
			break;
		}
		if (ftype>2) break;
		if (e) break;
		nb_frames++;
	}
	gf_m4v_parser_del(parser);
	if ((e==GF_OK) && (nb_frames>1)) {
		*score = o_start ? GF_FPROBE_MAYBE_NOT_SUPPORTED : GF_FPROBE_MAYBE_SUPPORTED;
		return "video/mpgv-es";
	}
	return NULL;
}