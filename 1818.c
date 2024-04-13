GF_Err gf_odf_av1_cfg_write(GF_AV1Config *cfg, u8 **outData, u32 *outSize) {
	GF_Err e;
	GF_BitStream *bs = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);
	*outSize = 0;
	*outData = NULL;
	e = gf_odf_av1_cfg_write_bs(cfg, bs);
	if (e == GF_OK)
		gf_bs_get_content(bs, outData, outSize);

	gf_bs_del(bs);
	return e;
}