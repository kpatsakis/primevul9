GF_Err gf_odf_vp_cfg_write(GF_VPConfig *cfg, u8 **outData, u32 *outSize, Bool is_v0)
{
	GF_Err e;
	GF_BitStream *bs = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);
	*outSize = 0;
	*outData = NULL;
	e = gf_odf_vp_cfg_write_bs(cfg, bs, is_v0);
	if (e==GF_OK)
		gf_bs_get_content(bs, outData, outSize);

	gf_bs_del(bs);
	return e;
}