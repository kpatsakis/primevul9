GF_Err gf_odf_vp_cfg_write_bs(GF_VPConfig *cfg, GF_BitStream *bs, Bool is_v0)
{
	gf_bs_write_int(bs, cfg->profile, 8);
	gf_bs_write_int(bs, cfg->level, 8);
	gf_bs_write_int(bs, cfg->bit_depth, 4);
	gf_bs_write_int(bs, cfg->chroma_subsampling, 3);
	gf_bs_write_int(bs, cfg->video_fullRange_flag, 1);
	gf_bs_write_int(bs, cfg->colour_primaries, 8);
	gf_bs_write_int(bs, cfg->transfer_characteristics, 8);
	gf_bs_write_int(bs, cfg->matrix_coefficients, 8);

	if (!is_v0) {
		if (cfg->codec_initdata_size) {
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[iso file] VP Configuration Box: invalid data, codec_initdata_size must be 0, was %d - ignoring\n", cfg->codec_initdata_size));
		}

		gf_bs_write_int(bs, (u16)0, 16);
	}

	return GF_OK;
}