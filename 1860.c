GF_Err gf_odf_tx3g_write(GF_TextSampleDescriptor *a, u8 **outData, u32 *outSize)
{
	u32 j;
	void gpp_write_rgba(GF_BitStream *bs, u32 col);
	void gpp_write_box(GF_BitStream *bs, GF_BoxRecord *rec);
	void gpp_write_style(GF_BitStream *bs, GF_StyleRecord *rec);
	GF_BitStream *bs = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);

	gf_bs_write_u8(bs, a->horiz_justif);
	gf_bs_write_u8(bs, a->vert_justif);
	gpp_write_rgba(bs, a->back_color);
	gpp_write_box(bs, &a->default_pos);
	gpp_write_style(bs, &a->default_style);

	gf_bs_write_u16(bs, a->font_count);
	for (j=0; j<a->font_count; j++) {
		gf_bs_write_u16(bs, a->fonts[j].fontID);
		if (a->fonts[j].fontName) {
			u32 len = (u32) strlen(a->fonts[j].fontName);
			gf_bs_write_u8(bs, len);
			gf_bs_write_data(bs, a->fonts[j].fontName, len);
		} else {
			gf_bs_write_u8(bs, 0);
		}
	}
	gf_bs_get_content(bs, outData, outSize);
	gf_bs_del(bs);
	return GF_OK;
}