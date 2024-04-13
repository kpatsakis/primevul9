GF_Err gf_odf_ac3_config_parse(u8 *dsi, u32 dsi_len, Bool is_ec3, GF_AC3Config *cfg)
{
	GF_BitStream *bs;
	GF_Err e;
	if (!cfg || !dsi) return GF_BAD_PARAM;
	bs = gf_bs_new(dsi, dsi_len, GF_BITSTREAM_READ);
	e = gf_odf_ac3_config_parse_bs(bs, is_ec3, cfg);
	gf_bs_del(bs);
	return e;
}