GF_AVCConfig *gf_odf_avc_cfg_read(u8 *dsi, u32 dsi_size)
{
	u32 i, count;
	GF_AVCConfig *avcc = gf_odf_avc_cfg_new();
	GF_BitStream *bs = gf_bs_new(dsi, dsi_size, GF_BITSTREAM_READ);
	avcc->configurationVersion = gf_bs_read_int(bs, 8);
	avcc->AVCProfileIndication  = gf_bs_read_int(bs, 8);
	avcc->profile_compatibility = gf_bs_read_int(bs, 8);
	avcc->AVCLevelIndication  = gf_bs_read_int(bs, 8);
	gf_bs_read_int(bs, 6);
	avcc->nal_unit_size = 1 + gf_bs_read_int(bs, 2);
	gf_bs_read_int(bs, 3);
	count = gf_bs_read_int(bs, 5);
	for (i=0; i<count; i++) {
		GF_NALUFFParam *sl = (GF_NALUFFParam *)gf_malloc(sizeof(GF_NALUFFParam));
		sl->size = gf_bs_read_int(bs, 16);
		sl->data = (char*)gf_malloc(sizeof(char)*sl->size);
		gf_bs_read_data(bs, sl->data, sl->size);
		gf_list_add(avcc->sequenceParameterSets, sl);
	}
	count = gf_bs_read_int(bs, 8);
	for (i=0; i<count; i++) {
		GF_NALUFFParam *sl = (GF_NALUFFParam *)gf_malloc(sizeof(GF_NALUFFParam));
		sl->size = gf_bs_read_int(bs, 16);
		sl->data = (char*)gf_malloc(sizeof(char)*sl->size);
		gf_bs_read_data(bs, sl->data, sl->size);
		gf_list_add(avcc->pictureParameterSets, sl);
	}
	if (gf_avc_is_rext_profile(avcc->AVCProfileIndication)) {
		gf_bs_read_int(bs, 6);
		avcc->chroma_format = gf_bs_read_int(bs, 2);
		gf_bs_read_int(bs, 5);
		avcc->luma_bit_depth = 8 + gf_bs_read_int(bs, 3);
		gf_bs_read_int(bs, 5);
		avcc->chroma_bit_depth = 8 + gf_bs_read_int(bs, 3);

		count = gf_bs_read_int(bs, 8);
		if (count) {
			avcc->sequenceParameterSetExtensions = gf_list_new();
			for (i=0; i<count; i++) {
				GF_NALUFFParam *sl = (GF_NALUFFParam *)gf_malloc(sizeof(GF_NALUFFParam));
				sl->size = gf_bs_read_u16(bs);
				sl->data = (char *)gf_malloc(sizeof(char) * sl->size);
				gf_bs_read_data(bs, sl->data, sl->size);
				gf_list_add(avcc->sequenceParameterSetExtensions, sl);
			}
		}
	}


	gf_bs_del(bs);
	return avcc;
}