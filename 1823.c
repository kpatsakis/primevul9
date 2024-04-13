GF_VPConfig *gf_odf_vp_cfg_read(u8 *dsi, u32 dsi_size)
{
	GF_BitStream *bs = gf_bs_new(dsi, dsi_size, GF_BITSTREAM_READ);
	GF_VPConfig *cfg = gf_odf_vp_cfg_read_bs(bs, GF_FALSE);
	gf_bs_del(bs);
	return cfg;
}