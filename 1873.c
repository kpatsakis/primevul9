GF_Err gf_odf_get_laser_config(GF_DefaultDescriptor *dsi, GF_LASERConfig *cfg)
{
	u32 to_skip;
	GF_BitStream *bs;

	if (!cfg) return GF_BAD_PARAM;
	memset(cfg, 0, sizeof(GF_LASERConfig));

	if (!dsi || !dsi->data || !dsi->dataLength) return GF_BAD_PARAM;
	bs = gf_bs_new(dsi->data, dsi->dataLength, GF_BITSTREAM_READ);
	memset(cfg, 0, sizeof(GF_LASERConfig));
	cfg->tag = GF_ODF_LASER_CFG_TAG;
	cfg->profile = gf_bs_read_int(bs, 8);
	cfg->level = gf_bs_read_int(bs, 8);
	/*cfg->reserved = */gf_bs_read_int(bs, 3);
	cfg->pointsCodec = gf_bs_read_int(bs, 2);
	cfg->pathComponents = gf_bs_read_int(bs, 4);
	cfg->fullRequestHost = gf_bs_read_int(bs, 1);
	if (gf_bs_read_int(bs, 1)) cfg->time_resolution = gf_bs_read_int(bs, 16);
	else cfg->time_resolution = 1000;
	cfg->colorComponentBits = 1 + gf_bs_read_int(bs, 4);
	cfg->resolution = gf_bs_read_int(bs, 4);
	if (cfg->resolution>7) cfg->resolution -= 16;
	cfg->coord_bits = gf_bs_read_int(bs, 5);
	cfg->scale_bits_minus_coord_bits = gf_bs_read_int(bs, 4);
	cfg->newSceneIndicator = gf_bs_read_int(bs, 1);
	/*reserved2*/ gf_bs_read_int(bs, 3);
	cfg->extensionIDBits = gf_bs_read_int(bs, 4);
	/*hasExtConfig - we just ignore it*/
	if (gf_bs_read_int(bs, 1)) {
		to_skip = gf_bs_read_vluimsbf5(bs);
		while (to_skip) {
			gf_bs_read_int(bs, 8);
			to_skip--;
		}
	}
	/*hasExtension - we just ignore it*/
	if (gf_bs_read_int(bs, 1)) {
		to_skip = gf_bs_read_vluimsbf5(bs);
		while (to_skip) {
			gf_bs_read_int(bs, 8);
			to_skip--;
		}
	}
	gf_bs_del(bs);
	return GF_OK;
}