GF_Err gf_odf_ac3_cfg_write(GF_AC3Config *cfg, u8 **data, u32 *size)
{
	GF_BitStream *bs = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);
	GF_Err e = gf_odf_ac3_cfg_write_bs(cfg, bs);

	gf_bs_get_content(bs, data, size);
	gf_bs_del(bs);
	return e;
}