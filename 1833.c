GF_VPConfig *gf_odf_vp_cfg_read_bs(GF_BitStream *bs, Bool is_v0)
{
	GF_VPConfig *cfg = gf_odf_vp_cfg_new();

	cfg->profile = gf_bs_read_int(bs, 8);
	cfg->level = gf_bs_read_int(bs, 8);

	cfg->bit_depth = gf_bs_read_int(bs, 4);
	cfg->chroma_subsampling = gf_bs_read_int(bs, 3);
	cfg->video_fullRange_flag = gf_bs_read_int(bs, 1);

	cfg->colour_primaries = gf_bs_read_int(bs, 8);
	cfg->transfer_characteristics = gf_bs_read_int(bs, 8);
	cfg->matrix_coefficients = gf_bs_read_int(bs, 8);

	if (is_v0)
		return cfg;

	cfg->codec_initdata_size = gf_bs_read_int(bs, 16);

	// must be 0 according to spec
	if (cfg->codec_initdata_size) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] VP Configuration Box: invalid data, codec_initdata_size must be 0, was %d\n", cfg->codec_initdata_size));
		gf_odf_vp_cfg_del(cfg);
		return NULL;
	}

	return cfg;
}