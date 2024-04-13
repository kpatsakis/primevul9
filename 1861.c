GF_Err gf_odf_vvc_cfg_write_bs(GF_VVCConfig *cfg, GF_BitStream *bs)
{
	u32 i, count;

	count = gf_list_count(cfg->param_array);

	if (!cfg->write_annex_b) {

		gf_bs_write_int(bs, 0xFF, 5);
		gf_bs_write_int(bs, cfg->nal_unit_size - 1, 2);
		gf_bs_write_int(bs, cfg->ptl_present, 1);

		if (cfg->ptl_present) {
			s32 idx;

			gf_bs_write_int(bs, cfg->ols_idx, 9);
			gf_bs_write_int(bs, cfg->numTemporalLayers, 3);
			gf_bs_write_int(bs, cfg->constantFrameRate, 2);
			gf_bs_write_int(bs, cfg->chroma_format, 2);
			gf_bs_write_int(bs, cfg->bit_depth - 8, 3);
			gf_bs_write_int(bs, 0xFF, 5);

			if (!cfg->general_constraint_info)
				cfg->num_constraint_info = 0;

			//write PTL
			gf_bs_write_int(bs, 0, 2);
			gf_bs_write_int(bs, cfg->num_constraint_info, 6);
			gf_bs_write_int(bs, cfg->general_profile_idc, 7);
			gf_bs_write_int(bs, cfg->general_tier_flag, 1);
			gf_bs_write_u8(bs, cfg->general_level_idc);
			gf_bs_write_int(bs, cfg->ptl_frame_only_constraint, 1);
			gf_bs_write_int(bs, cfg->ptl_multilayer_enabled, 1);

			if (cfg->num_constraint_info) {
				gf_bs_write_data(bs, cfg->general_constraint_info, cfg->num_constraint_info - 1);
				gf_bs_write_int(bs, cfg->general_constraint_info[cfg->num_constraint_info - 1], 6);
			} else {
				gf_bs_write_int(bs, 0, 6);
			}

			for (idx=cfg->numTemporalLayers-2; idx>=0; idx--) {
				u8 val = cfg->ptl_sublayer_present_mask & (1<<idx);
				gf_bs_write_int(bs, val ? 1 : 0, 1);
			}
			for (idx=cfg->numTemporalLayers; idx<=8 && cfg->numTemporalLayers>1; idx++) {
				gf_bs_write_int(bs, 0, 1);
			}
			for (idx=cfg->numTemporalLayers-2; idx>=0; idx--) {
				if (cfg->ptl_sublayer_present_mask & (1<<idx))
					gf_bs_write_u8(bs, cfg->sublayer_level_idc[idx]);
			}
			if (!cfg->sub_profiles_idc) cfg->num_sub_profiles = 0;
			gf_bs_write_u8(bs, cfg->num_sub_profiles);
			for (idx=0; idx<cfg->num_sub_profiles; idx++) {
				gf_bs_write_u32(bs, cfg->sub_profiles_idc[idx]);
			}
			//end PTL

			gf_bs_write_u16(bs, cfg->maxPictureWidth);
			gf_bs_write_u16(bs, cfg->maxPictureHeight);
			gf_bs_write_u16(bs, cfg->avgFrameRate);
		}
		gf_bs_write_int(bs, count, 8);
	}

	for (i=0; i<count; i++) {
		u32 nalucount, j;
		GF_NALUFFParamArray *ar = (GF_NALUFFParamArray*)gf_list_get(cfg->param_array, i);

		nalucount = gf_list_count(ar->nalus);
		if (!cfg->write_annex_b) {
			gf_bs_write_int(bs, ar->array_completeness, 1);
			gf_bs_write_int(bs, 0, 2);
			gf_bs_write_int(bs, ar->type, 5);

			if ((ar->type != GF_VVC_NALU_DEC_PARAM) && (ar->type != GF_VVC_NALU_OPI))
				gf_bs_write_int(bs, nalucount, 16);
			else
				nalucount = 1;
		}

		for (j=0; j<nalucount; j++) {
			GF_NALUFFParam *sl = (GF_NALUFFParam *)gf_list_get(ar->nalus, j);
			if (!cfg->write_annex_b) {
				gf_bs_write_int(bs, sl->size, 16);
			} else {
				gf_bs_write_u32(bs, 1);
			}
			gf_bs_write_data(bs, sl->data, sl->size);
		}
	}
	return GF_OK;
}