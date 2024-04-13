void gf_odf_vp_cfg_del(GF_VPConfig *cfg)
{
	if (!cfg) return;

	if (cfg->codec_initdata) {
		gf_free(cfg->codec_initdata);
		cfg->codec_initdata = NULL;
	}

	gf_free(cfg);
}