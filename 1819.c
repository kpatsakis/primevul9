GF_DOVIDecoderConfigurationRecord *gf_odf_dovi_cfg_read_bs(GF_BitStream *bs)
{
	GF_DOVIDecoderConfigurationRecord *cfg;
	GF_SAFEALLOC(cfg, GF_DOVIDecoderConfigurationRecord);

	cfg->dv_version_major = gf_bs_read_u8(bs);
	cfg->dv_version_minor = gf_bs_read_u8(bs);
	cfg->dv_profile = gf_bs_read_int(bs, 7);
	cfg->dv_level = gf_bs_read_int(bs, 6);
	cfg->rpu_present_flag = gf_bs_read_int(bs, 1);
	cfg->el_present_flag = gf_bs_read_int(bs, 1);
	cfg->bl_present_flag = gf_bs_read_int(bs, 1);
	{
		int i = 0;
		u32 data[5];
		memset(data, 0, sizeof(data));
		gf_bs_read_data(bs, (char*)data, 20);
		for (i = 0; i < 5; ++i) {
			if (data[i] != 0) {
				GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("[odf_cfg] dovi config reserved bytes are not zero\n"));
			}
		}
	}
	return cfg;
}