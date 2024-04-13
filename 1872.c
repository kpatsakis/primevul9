GF_Err gf_odf_encode_ui_config(GF_UIConfig *cfg, GF_DefaultDescriptor **out_dsi)
{
	u32 i, len;
	GF_BitStream *bs;
	GF_DefaultDescriptor *dsi;
	if (!out_dsi || (cfg->tag != GF_ODF_UI_CFG_TAG)) return GF_BAD_PARAM;

	*out_dsi = NULL;
	if (!cfg->deviceName) return GF_OK;

	bs = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);
	len = (u32) strlen(cfg->deviceName);
	gf_bs_write_int(bs, len, 8);
	for (i=0; i<len; i++) gf_bs_write_int(bs, cfg->deviceName[i], 8);
	if (!stricmp(cfg->deviceName, "StringSensor")) {
		/*fixme - this should be UTF-8 chars*/
		if (cfg->delChar || cfg->termChar) {
			gf_bs_write_int(bs, cfg->termChar, 8);
			gf_bs_write_int(bs, cfg->delChar, 8);
		}
	}
	if (cfg->ui_data) gf_bs_write_data(bs, cfg->ui_data, cfg->ui_data_length);

	dsi = (GF_DefaultDescriptor *) gf_odf_desc_new(GF_ODF_DSI_TAG);
	gf_bs_get_content(bs, &dsi->data, &dsi->dataLength);
	gf_bs_del(bs);
	*out_dsi = dsi;
	return GF_OK;
}