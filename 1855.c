void gf_odf_avc_cfg_del(GF_AVCConfig *cfg)
{
	if (!cfg) return;
	while (gf_list_count(cfg->sequenceParameterSets)) {
		GF_NALUFFParam *sl = (GF_NALUFFParam *)gf_list_get(cfg->sequenceParameterSets, 0);
		gf_list_rem(cfg->sequenceParameterSets, 0);
		if (sl->data) gf_free(sl->data);
		gf_free(sl);
	}
	gf_list_del(cfg->sequenceParameterSets);
	cfg->sequenceParameterSets = NULL;

	while (gf_list_count(cfg->pictureParameterSets)) {
		GF_NALUFFParam *sl = (GF_NALUFFParam *)gf_list_get(cfg->pictureParameterSets, 0);
		gf_list_rem(cfg->pictureParameterSets, 0);
		if (sl->data) gf_free(sl->data);
		gf_free(sl);
	}
	gf_list_del(cfg->pictureParameterSets);
	cfg->pictureParameterSets = NULL;

	if (cfg->sequenceParameterSetExtensions) {
		while (gf_list_count(cfg->sequenceParameterSetExtensions)) {
			GF_NALUFFParam *sl = (GF_NALUFFParam *)gf_list_get(cfg->sequenceParameterSetExtensions, 0);
			gf_list_rem(cfg->sequenceParameterSetExtensions, 0);
			if (sl->data) gf_free(sl->data);
			gf_free(sl);
		}
		gf_list_del(cfg->sequenceParameterSetExtensions);
		cfg->sequenceParameterSetExtensions = NULL;
	}
	gf_free(cfg);
}