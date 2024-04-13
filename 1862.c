GF_HEVCConfig *gf_odf_hevc_cfg_new()
{
	GF_HEVCConfig *cfg;
	GF_SAFEALLOC(cfg, GF_HEVCConfig);
	if (!cfg) return NULL;
	cfg->param_array = gf_list_new();
	cfg->nal_unit_size = 4;
	return cfg;
}