void gf_odf_av1_cfg_del(GF_AV1Config *cfg)
{
	if (!cfg) return;
	while (gf_list_count(cfg->obu_array)) {
		GF_AV1_OBUArrayEntry *a = (GF_AV1_OBUArrayEntry*)gf_list_get(cfg->obu_array, 0);
		if (a->obu) gf_free(a->obu);
		gf_list_rem(cfg->obu_array, 0);
		gf_free(a);
	}
	gf_list_del(cfg->obu_array);
	gf_free(cfg);
}