GF_AV1Config *gf_odf_av1_cfg_read(u8 *dsi, u32 dsi_size)
{
	GF_BitStream *bs = gf_bs_new(dsi, dsi_size, GF_BITSTREAM_READ);
	GF_AV1Config *cfg = gf_odf_av1_cfg_read_bs(bs);
	gf_bs_del(bs);
	return cfg;
}