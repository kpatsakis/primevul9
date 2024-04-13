GF_Err gf_odf_av1_cfg_write_bs(GF_AV1Config *cfg, GF_BitStream *bs)
{
	u32 i = 0;
	gf_bs_write_int(bs, cfg->marker, 1); assert(cfg->marker == 1);
	gf_bs_write_int(bs, cfg->version, 7); assert(cfg->version == 1);
	gf_bs_write_int(bs, cfg->seq_profile, 3);
	gf_bs_write_int(bs, cfg->seq_level_idx_0, 5);
	gf_bs_write_int(bs, cfg->seq_tier_0, 1);
	gf_bs_write_int(bs, cfg->high_bitdepth, 1);
	gf_bs_write_int(bs, cfg->twelve_bit, 1);
	gf_bs_write_int(bs, cfg->monochrome, 1);
	gf_bs_write_int(bs, cfg->chroma_subsampling_x, 1);
	gf_bs_write_int(bs, cfg->chroma_subsampling_y, 1);
	gf_bs_write_int(bs, cfg->chroma_sample_position, 2);
	gf_bs_write_int(bs, 0, 3); /*reserved*/
	gf_bs_write_int(bs, cfg->initial_presentation_delay_present, 1);
	gf_bs_write_int(bs, cfg->initial_presentation_delay_minus_one, 4); /*TODO: compute initial_presentation_delay_minus_one*/
	for (i = 0; i < gf_list_count(cfg->obu_array); ++i) {
		GF_AV1_OBUArrayEntry *a = gf_list_get(cfg->obu_array, i);
		gf_bs_write_data(bs, a->obu, (u32)a->obu_length); //TODO: we are supposed to omit the size on the last OBU...
	}
	return GF_OK;
}