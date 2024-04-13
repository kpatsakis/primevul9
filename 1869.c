GF_HEVCConfig *gf_odf_hevc_cfg_read(u8 *dsi, u32 dsi_size, Bool is_lhvc)
{
	GF_BitStream *bs = gf_bs_new(dsi, dsi_size, GF_BITSTREAM_READ);
	GF_HEVCConfig *cfg = gf_odf_hevc_cfg_read_bs(bs, is_lhvc);
	gf_bs_del(bs);
	return cfg;
}