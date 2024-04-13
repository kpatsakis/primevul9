GF_VVCConfig *gf_odf_vvc_cfg_new()
{
	GF_VVCConfig *cfg;
	GF_SAFEALLOC(cfg, GF_VVCConfig);
	if (!cfg) return NULL;
	cfg->param_array = gf_list_new();
	cfg->nal_unit_size = 4;
	cfg->chroma_format = 1;
	cfg->bit_depth = 8;
	return cfg;
}