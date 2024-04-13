GF_VVCConfig *gf_odf_vvc_cfg_read_bs(GF_BitStream *bs)
{
	u32 i, count;
	GF_VVCConfig *cfg = gf_odf_vvc_cfg_new();

	gf_bs_read_int(bs, 5);
	cfg->nal_unit_size = 1 + gf_bs_read_int(bs, 2);
	cfg->ptl_present = gf_bs_read_int(bs, 1);

	if (cfg->ptl_present) {
		s32 j;

		cfg->ols_idx = gf_bs_read_int(bs, 9);
		cfg->numTemporalLayers = gf_bs_read_int(bs, 3);
		cfg->constantFrameRate = gf_bs_read_int(bs, 2);
		cfg->chroma_format = gf_bs_read_int(bs, 2);
		cfg->bit_depth = 8 + gf_bs_read_int(bs, 3);
		gf_bs_read_int(bs, 5);

		//parse PTL
		gf_bs_read_int(bs, 2);
		cfg->num_constraint_info = gf_bs_read_int(bs, 6);
		cfg->general_profile_idc = gf_bs_read_int(bs, 7);
		cfg->general_tier_flag = gf_bs_read_int(bs, 1);
		cfg->general_level_idc = gf_bs_read_u8(bs);
		cfg->ptl_frame_only_constraint = gf_bs_read_int(bs, 1);
		cfg->ptl_multilayer_enabled = gf_bs_read_int(bs, 1);

		if (cfg->num_constraint_info) {
			cfg->general_constraint_info = gf_malloc(sizeof(u8)*cfg->num_constraint_info);
			if (!cfg->general_constraint_info) {
				gf_free(cfg);
				GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] alloc failed while parsing vvc config\n"));
				return NULL;
			}
			gf_bs_read_data(bs, cfg->general_constraint_info, cfg->num_constraint_info - 1);
			cfg->general_constraint_info[cfg->num_constraint_info-1] =  gf_bs_read_int(bs, 6);
		} else {
			//forbidden in spec!
			gf_bs_read_int(bs, 6);
		}

		cfg->ptl_sublayer_present_mask = 0;
		for (j=cfg->numTemporalLayers-2; j>=0; j--) {
			u32 val = gf_bs_read_int(bs, 1);
			cfg->ptl_sublayer_present_mask |= val << j;
		}
		for (j=cfg->numTemporalLayers; j<=8 && cfg->numTemporalLayers>1; j++) {
			gf_bs_read_int(bs, 1);
		}
		for (j=cfg->numTemporalLayers-2; j>=0; j--) {
			if (cfg->ptl_sublayer_present_mask & (1<<j)) {
				cfg->sublayer_level_idc[j] = gf_bs_read_u8(bs);
			}
		}
		cfg->num_sub_profiles = gf_bs_read_u8(bs);
		if (cfg->num_sub_profiles) {
			cfg->sub_profiles_idc = gf_malloc(sizeof(u32)*cfg->num_sub_profiles);
			if (!cfg->sub_profiles_idc) {
				gf_free(cfg->general_constraint_info);
				gf_free(cfg);
				GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] alloc failed while parsing vvc config\n"));
				return NULL;
			}
		}
		for (i=0; i<cfg->num_sub_profiles; i++) {
			cfg->sub_profiles_idc[i] = gf_bs_read_u32(bs);
		}

		//end PTL

		cfg->maxPictureWidth = gf_bs_read_u16(bs);
		cfg->maxPictureHeight = gf_bs_read_u16(bs);
		cfg->avgFrameRate = gf_bs_read_u16(bs);
	}

	count = gf_bs_read_int(bs, 8);
	for (i=0; i<count; i++) {
		u32 nalucount, j;
		GF_NALUFFParamArray *ar;
		GF_SAFEALLOC(ar, GF_NALUFFParamArray);
		if (!ar) {
			gf_odf_vvc_cfg_del(cfg);
			GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] alloc failed while parsing vvc config\n"));
			return NULL;
		}
		ar->nalus = gf_list_new();
		gf_list_add(cfg->param_array, ar);

		ar->array_completeness = gf_bs_read_int(bs, 1);
		gf_bs_read_int(bs, 2);
		ar->type = gf_bs_read_int(bs, 5);

		if ((ar->type != GF_VVC_NALU_DEC_PARAM) && (ar->type != GF_VVC_NALU_OPI))
			nalucount = gf_bs_read_int(bs, 16);
		else
			nalucount = 1;
			
		for (j=0; j<nalucount; j++) {
			GF_NALUFFParam *sl;
			u32 size = gf_bs_read_int(bs, 16);
			if (size>gf_bs_available(bs)) {
				GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Wrong param set size %d\n", size));
				gf_odf_vvc_cfg_del(cfg);
				return NULL;
			}
			GF_SAFEALLOC(sl, GF_NALUFFParam );
			if (!sl) {
				gf_odf_vvc_cfg_del(cfg);
				GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] alloc failed while parsing vvc config\n"));
				return NULL;
			}

			sl->size = size;
			sl->data = (char *)gf_malloc(sizeof(char) * sl->size);
			if (!sl->data) {
				gf_free(sl);
				gf_odf_vvc_cfg_del(cfg);
				GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] alloc failed while parsing vvc config\n"));
				return NULL;
			}
			gf_bs_read_data(bs, sl->data, sl->size);
			gf_list_add(ar->nalus, sl);
		}
	}
	return cfg;
}