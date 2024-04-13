GF_Err gf_odf_hevc_cfg_write_bs(GF_HEVCConfig *cfg, GF_BitStream *bs)
{
	u32 i, count;

	count = gf_list_count(cfg->param_array);

	if (!cfg->write_annex_b) {
		gf_bs_write_int(bs, cfg->configurationVersion, 8);

		if (!cfg->is_lhvc) {
			gf_bs_write_int(bs, cfg->profile_space, 2);
			gf_bs_write_int(bs, cfg->tier_flag, 1);
			gf_bs_write_int(bs, cfg->profile_idc, 5);
			gf_bs_write_int(bs, cfg->general_profile_compatibility_flags, 32);
			gf_bs_write_int(bs, cfg->progressive_source_flag, 1);
			gf_bs_write_int(bs, cfg->interlaced_source_flag, 1);
			gf_bs_write_int(bs, cfg->non_packed_constraint_flag, 1);
			gf_bs_write_int(bs, cfg->frame_only_constraint_flag, 1);
			/*only lowest 44 bits used*/
			gf_bs_write_long_int(bs, cfg->constraint_indicator_flags, 44);
			gf_bs_write_int(bs, cfg->level_idc, 8);
		}

		gf_bs_write_int(bs, 0xFF, 4);
		gf_bs_write_int(bs, cfg->min_spatial_segmentation_idc, 12);

		gf_bs_write_int(bs, 0xFF, 6);
		gf_bs_write_int(bs, cfg->parallelismType, 2);

		if (!cfg->is_lhvc) {
			gf_bs_write_int(bs, 0xFF, 6);
			gf_bs_write_int(bs, cfg->chromaFormat, 2);
			gf_bs_write_int(bs, 0xFF, 5);
			gf_bs_write_int(bs, cfg->luma_bit_depth-8, 3);
			gf_bs_write_int(bs, 0xFF, 5);
			gf_bs_write_int(bs, cfg->chroma_bit_depth-8, 3);
			gf_bs_write_int(bs, cfg->avgFrameRate, 16);

			gf_bs_write_int(bs, cfg->constantFrameRate, 2);
		} else {
			gf_bs_write_int(bs, 0xFF, 2);
		}

		gf_bs_write_int(bs, cfg->numTemporalLayers, 3);
		gf_bs_write_int(bs, cfg->temporalIdNested, 1);
		gf_bs_write_int(bs, cfg->nal_unit_size - 1, 2);

		gf_bs_write_int(bs, count, 8);
	}

	for (i=0; i<count; i++) {
		u32 nalucount, j;
		GF_NALUFFParamArray *ar = (GF_NALUFFParamArray*)gf_list_get(cfg->param_array, i);

		nalucount = gf_list_count(ar->nalus);
		if (!cfg->write_annex_b) {
			gf_bs_write_int(bs, ar->array_completeness, 1);
			gf_bs_write_int(bs, 0, 1);
			gf_bs_write_int(bs, ar->type, 6);
			gf_bs_write_int(bs, nalucount, 16);
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