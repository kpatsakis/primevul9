GF_Err gf_odf_write_base_descriptor(GF_BitStream *bs, u8 tag, u32 size)
{
	u32 length;
	unsigned char vals[4];

	if (!tag ) return GF_BAD_PARAM;

	length = size;
	vals[3] = (unsigned char) (length & 0x7f);
	length >>= 7;
	vals[2] = (unsigned char) ((length & 0x7f) | 0x80);
	length >>= 7;
	vals[1] = (unsigned char) ((length & 0x7f) | 0x80);
	length >>= 7;
	vals[0] = (unsigned char) ((length & 0x7f) | 0x80);

	gf_bs_write_int(bs, tag, 8);
	if (size < 0x00000080) {
		gf_bs_write_int(bs, vals[3], 8);
	} else if (size < 0x00004000) {
		gf_bs_write_int(bs, vals[2], 8);
		gf_bs_write_int(bs, vals[3], 8);
	} else if (size < 0x00200000) {
		gf_bs_write_int(bs, vals[1], 8);
		gf_bs_write_int(bs, vals[2], 8);
		gf_bs_write_int(bs, vals[3], 8);
	} else if (size < 0x10000000) {
		gf_bs_write_int(bs, vals[0], 8);
		gf_bs_write_int(bs, vals[1], 8);
		gf_bs_write_int(bs, vals[2], 8);
		gf_bs_write_int(bs, vals[3], 8);
	} else {
		return GF_ODF_INVALID_DESCRIPTOR;
	}
	return GF_OK;
}