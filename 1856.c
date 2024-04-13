GF_Err gf_odf_avc_cfg_write_bs(GF_AVCConfig *cfg, GF_BitStream *bs)
{
	u32 i, count;

	if (!cfg) return GF_BAD_PARAM;

	count = gf_list_count(cfg->sequenceParameterSets);

	if (!cfg->write_annex_b) {
		gf_bs_write_int(bs, cfg->configurationVersion, 8);
		gf_bs_write_int(bs, cfg->AVCProfileIndication , 8);
		gf_bs_write_int(bs, cfg->profile_compatibility, 8);
		gf_bs_write_int(bs, cfg->AVCLevelIndication, 8);
		gf_bs_write_int(bs, 0x3F, 6);
		gf_bs_write_int(bs, cfg->nal_unit_size - 1, 2);
		gf_bs_write_int(bs, 0x7, 3);
		gf_bs_write_int(bs, count, 5);
	}
	for (i=0; i<count; i++) {
		GF_NALUFFParam *sl = (GF_NALUFFParam *)gf_list_get(cfg->sequenceParameterSets, i);
		if (!cfg->write_annex_b) {
			gf_bs_write_u16(bs, sl->size);
		} else {
			gf_bs_write_u32(bs, 1);
		}
		gf_bs_write_data(bs, sl->data, sl->size);
	}
	count = gf_list_count(cfg->pictureParameterSets);
	if (!cfg->write_annex_b) {
		gf_bs_write_int(bs, count, 8);
	}
	for (i=0; i<count; i++) {
		GF_NALUFFParam *sl = (GF_NALUFFParam *)gf_list_get(cfg->pictureParameterSets, i);
		if (!cfg->write_annex_b) {
			gf_bs_write_u16(bs, sl->size);
		} else {
			gf_bs_write_u32(bs, 1);
		}
		gf_bs_write_data(bs, sl->data, sl->size);
	}
	if (gf_avc_is_rext_profile(cfg->AVCProfileIndication)) {
		if (!cfg->write_annex_b) {
			gf_bs_write_int(bs, 0xFF, 6);
			gf_bs_write_int(bs, cfg->chroma_format, 2);
			gf_bs_write_int(bs, 0xFF, 5);
			gf_bs_write_int(bs, cfg->luma_bit_depth - 8, 3);
			gf_bs_write_int(bs, 0xFF, 5);
			gf_bs_write_int(bs, cfg->chroma_bit_depth - 8, 3);
		}
		count = cfg->sequenceParameterSetExtensions ? gf_list_count(cfg->sequenceParameterSetExtensions) : 0;
		if (!cfg->write_annex_b) {
			gf_bs_write_u8(bs, count);
		}
		for (i=0; i<count; i++) {
			GF_NALUFFParam *sl = (GF_NALUFFParam *) gf_list_get(cfg->sequenceParameterSetExtensions, i);
			if (!cfg->write_annex_b) {
				gf_bs_write_u16(bs, sl->size);
			} else {
				gf_bs_write_u32(bs, 1);
			}
			gf_bs_write_data(bs, sl->data, sl->size);
		}
	}
	return GF_OK;
}