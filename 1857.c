GF_TextSampleDescriptor *gf_odf_tx3g_read(u8 *dsi, u32 dsi_size)
{
	u32 i;
	u32 gpp_read_rgba(GF_BitStream *bs);
	void gpp_read_style(GF_BitStream *bs, GF_StyleRecord *rec);
	void gpp_read_box(GF_BitStream *bs, GF_BoxRecord *rec);

	GF_TextSampleDescriptor *txtc = (GF_TextSampleDescriptor *) gf_odf_new_tx3g();
	GF_BitStream *bs = gf_bs_new(dsi, dsi_size, GF_BITSTREAM_READ);

	txtc->horiz_justif = gf_bs_read_int(bs, 8);
	txtc->vert_justif  = gf_bs_read_int(bs, 8);
	txtc->back_color = gpp_read_rgba(bs);
	gpp_read_box(bs, &txtc->default_pos);
	gpp_read_style(bs, &txtc->default_style);
	txtc->font_count = gf_bs_read_u16(bs);
	txtc->fonts = gf_malloc(sizeof(GF_FontRecord)*txtc->font_count);
	for (i=0; i<txtc->font_count; i++) {
		u8 len;
		txtc->fonts[i].fontID = gf_bs_read_u16(bs);
		len = gf_bs_read_u8(bs);
		txtc->fonts[i].fontName = gf_malloc(sizeof(char)*(len+1));
		gf_bs_read_data(bs, txtc->fonts[i].fontName, len);
		txtc->fonts[i].fontName[len] = 0;
	}
	gf_bs_del(bs);
	return txtc;
}