GF_AVCConfig *gf_odf_avc_cfg_new()
{
	GF_AVCConfig *cfg;
	GF_SAFEALLOC(cfg, GF_AVCConfig);
	if (!cfg) return NULL;
	cfg->sequenceParameterSets = gf_list_new();
	cfg->pictureParameterSets = gf_list_new();
	cfg->AVCLevelIndication = 1;
	cfg->chroma_format = 1;
	cfg->chroma_bit_depth = 8;
	cfg->luma_bit_depth = 8;
	return cfg;
}