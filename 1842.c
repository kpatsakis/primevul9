GF_Err gf_odf_get_text_config(u8 *data, u32 data_len, u32 codecid, GF_TextConfig *cfg)
{
	u32 i;
	Bool has_alt_format;
#ifndef GPAC_DISABLE_ISOM
	Bool has_sd;
	u32 j;
#endif
	GF_Err e;
	GF_BitStream *bs;
	if (data || data_len || !cfg) return GF_BAD_PARAM;
	if (codecid != GF_CODECID_TEXT_MPEG4) return GF_NOT_SUPPORTED;

	/*reset*/
	ResetTextConfig(cfg);
	bs = gf_bs_new(data, data_len, GF_BITSTREAM_READ);

	e = GF_OK;
	cfg->Base3GPPFormat = gf_bs_read_int(bs, 8);
	cfg->MPEGExtendedFormat = gf_bs_read_int(bs, 8);
	cfg->profileLevel = gf_bs_read_int(bs, 8);
	cfg->timescale = gf_bs_read_int(bs, 24);
	has_alt_format = (Bool)gf_bs_read_int(bs, 1);
	cfg->sampleDescriptionFlags = gf_bs_read_int(bs, 2);
#ifndef GPAC_DISABLE_ISOM
	has_sd = (Bool)gf_bs_read_int(bs, 1);
#else
	gf_bs_read_int(bs, 1);
#endif
	cfg->has_vid_info = (Bool)gf_bs_read_int(bs, 1);
	gf_bs_read_int(bs, 3);
	cfg->layer = gf_bs_read_int(bs, 8);
	cfg->text_width = gf_bs_read_int(bs, 16);
	cfg->text_height = gf_bs_read_int(bs, 16);
	if (has_alt_format) {
		cfg->nb_compatible_formats = gf_bs_read_int(bs, 8);
		for (i=0; i<cfg->nb_compatible_formats; i++) cfg->compatible_formats[i] = gf_bs_read_int(bs, 8);
	}
#ifndef GPAC_DISABLE_ISOM
	if (has_sd) {
		u8 sample_index;
		GF_TextSampleDescriptor *txdesc;
		GF_Tx3gSampleEntryBox *a;
		s64 avail;
		u32 nb_desc = gf_bs_read_int(bs, 8);

		/*parse TTU[5]s*/
		avail = (s64) gf_bs_available(bs);
		for (i=0; i<nb_desc; i++) {
			sample_index = gf_bs_read_int(bs, 8);
			avail -= 1;
			e = gf_isom_box_parse((GF_Box **) &a, bs);
			if (e) goto exit;
			avail -= (s32) a->size;

			if (avail<0) {
				e = GF_NON_COMPLIANT_BITSTREAM;
				goto exit;
			}
			txdesc = (GF_TextSampleDescriptor *)gf_malloc(sizeof(GF_TextSampleDescriptor));
			txdesc->sample_index = sample_index;
			txdesc->displayFlags = a->displayFlags;
			txdesc->back_color = a->back_color;
			txdesc->default_pos = a->default_box;
			txdesc->default_style = a->default_style;
			txdesc->vert_justif = a->vertical_justification;
			txdesc->horiz_justif = a->horizontal_justification;
			txdesc->font_count = a->font_table ? a->font_table->entry_count : 0;
			if (txdesc->font_count) {
				txdesc->fonts = (GF_FontRecord*)gf_malloc(sizeof(GF_FontRecord)*txdesc->font_count);
				for (j=0; j<txdesc->font_count; j++) {
					txdesc->fonts[j].fontID = a->font_table->fonts[j].fontID;
					txdesc->fonts[j].fontName = a->font_table->fonts[j].fontName ? gf_strdup(a->font_table->fonts[j].fontName) : NULL;
				}
			}
			gf_list_add(cfg->sample_descriptions, txdesc);
			gf_isom_box_del((GF_Box *)a);
		}
	}
#endif

	if (cfg->has_vid_info) {
		cfg->video_width = gf_bs_read_int(bs, 16);
		cfg->video_height = gf_bs_read_int(bs, 16);
		cfg->horiz_offset = gf_bs_read_int(bs, 16);
		cfg->vert_offset = gf_bs_read_int(bs, 16);
	}

#ifndef GPAC_DISABLE_ISOM
exit:
#endif
	gf_bs_del(bs);
	if (e) ResetTextConfig(cfg);
	return e;
}