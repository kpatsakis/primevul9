GF_VPConfig *gf_odf_vp_cfg_new()
{
	GF_VPConfig *cfg;
	GF_SAFEALLOC(cfg, GF_VPConfig);
	if (!cfg) return NULL;
	cfg->codec_initdata_size = 0;
	cfg->codec_initdata = NULL;
	return cfg;
}