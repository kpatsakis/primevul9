GF_Err gf_odf_avc_cfg_write(GF_AVCConfig *cfg, u8 **outData, u32 *outSize)
{
	GF_BitStream *bs = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);
	gf_odf_avc_cfg_write_bs(cfg, bs);
	*outSize = 0;
	*outData = NULL;
	gf_bs_get_content(bs, outData, outSize);
	gf_bs_del(bs);
	return GF_OK;
}