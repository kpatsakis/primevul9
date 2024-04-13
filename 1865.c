GF_AV1Config *gf_odf_av1_cfg_new()
{
	GF_AV1Config *cfg;
	GF_SAFEALLOC(cfg, GF_AV1Config);
	if (!cfg) return NULL;
	cfg->marker = 1;
	cfg->version = 1;
	cfg->initial_presentation_delay_minus_one = 0;
	cfg->obu_array = gf_list_new();
	return cfg;
}