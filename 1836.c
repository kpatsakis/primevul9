GF_HEVCConfig *gf_odf_hevc_cfg_read_bs(GF_BitStream *bs, Bool is_lhvc)
{
	u32 i, count;
	GF_HEVCConfig *cfg = gf_odf_hevc_cfg_new();

	cfg->is_lhvc = is_lhvc;

	cfg->configurationVersion = gf_bs_read_int(bs, 8);

	if (!is_lhvc) {
		cfg->profile_space = gf_bs_read_int(bs, 2);
		cfg->tier_flag = gf_bs_read_int(bs, 1);
		cfg->profile_idc = gf_bs_read_int(bs, 5);
		cfg->general_profile_compatibility_flags = gf_bs_read_int(bs, 32);

		cfg->progressive_source_flag = gf_bs_read_int(bs, 1);
		cfg->interlaced_source_flag = gf_bs_read_int(bs, 1);
		cfg->non_packed_constraint_flag = gf_bs_read_int(bs, 1);
		cfg->frame_only_constraint_flag = gf_bs_read_int(bs, 1);
		/*only lowest 44 bits used*/
		cfg->constraint_indicator_flags = gf_bs_read_long_int(bs, 44);
		cfg->level_idc = gf_bs_read_int(bs, 8);
	}

	gf_bs_read_int(bs, 4); //reserved
	cfg->min_spatial_segmentation_idc = gf_bs_read_int(bs, 12);

	gf_bs_read_int(bs, 6);//reserved
	cfg->parallelismType = gf_bs_read_int(bs, 2);

	if (!is_lhvc) {
		gf_bs_read_int(bs, 6);
		cfg->chromaFormat = gf_bs_read_int(bs, 2);
		gf_bs_read_int(bs, 5);
		cfg->luma_bit_depth = gf_bs_read_int(bs, 3) + 8;
		gf_bs_read_int(bs, 5);
		cfg->chroma_bit_depth = gf_bs_read_int(bs, 3) + 8;
		cfg->avgFrameRate = gf_bs_read_int(bs, 16);

		cfg->constantFrameRate = gf_bs_read_int(bs, 2);
	} else {
		gf_bs_read_int(bs, 2); //reserved
	}

	cfg->numTemporalLayers = gf_bs_read_int(bs, 3);
	cfg->temporalIdNested = gf_bs_read_int(bs, 1);

	cfg->nal_unit_size = 1 + gf_bs_read_int(bs, 2);

	count = gf_bs_read_int(bs, 8);
	for (i=0; i<count; i++) {
		u32 nalucount, j;
		GF_NALUFFParamArray *ar;
		GF_SAFEALLOC(ar, GF_NALUFFParamArray);
		if (!ar) {
			gf_odf_hevc_cfg_del(cfg);
			return NULL;
		}
		ar->nalus = gf_list_new();
		gf_list_add(cfg->param_array, ar);

		ar->array_completeness = gf_bs_read_int(bs, 1);
		gf_bs_read_int(bs, 1);
		ar->type = gf_bs_read_int(bs, 6);
		nalucount = gf_bs_read_int(bs, 16);
		for (j=0; j<nalucount; j++) {
			GF_NALUFFParam *sl;
			u32 size = gf_bs_read_int(bs, 16);
			if (size>gf_bs_available(bs)) {
				GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Wrong param set size %d\n", size));
				gf_odf_hevc_cfg_del(cfg);
				return NULL;
			}
			GF_SAFEALLOC(sl, GF_NALUFFParam );
			if (!sl) {
				gf_odf_hevc_cfg_del(cfg);
				return NULL;
			}

			sl->size = size;
			sl->data = (char *)gf_malloc(sizeof(char) * sl->size);
			gf_bs_read_data(bs, sl->data, sl->size);
			gf_list_add(ar->nalus, sl);
		}
	}
	return cfg;
}