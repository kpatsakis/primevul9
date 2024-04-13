GF_Err gf_odf_get_ui_config(GF_DefaultDescriptor *dsi, GF_UIConfig *cfg)
{
	u32 len, i;
	GF_BitStream *bs;
	if (!dsi || !dsi->data || !dsi->dataLength || !cfg) return GF_BAD_PARAM;
	memset(cfg, 0, sizeof(GF_UIConfig));
	cfg->tag = GF_ODF_UI_CFG_TAG;
	bs = gf_bs_new(dsi->data, dsi->dataLength, GF_BITSTREAM_READ);
	len = gf_bs_read_int(bs, 8);
	cfg->deviceName = (char*)gf_malloc(sizeof(char) * (len+1));
	for (i=0; i<len; i++) cfg->deviceName[i] = gf_bs_read_int(bs, 8);
	cfg->deviceName[i] = 0;

	if (!stricmp(cfg->deviceName, "StringSensor") && gf_bs_available(bs)) {
		cfg->termChar = gf_bs_read_int(bs, 8);
		cfg->delChar = gf_bs_read_int(bs, 8);
	}
	gf_bs_del(bs);
	return GF_OK;
}