void gf_odf_vvc_cfg_del(GF_VVCConfig *cfg)
{
	if (!cfg) return;
	while (gf_list_count(cfg->param_array)) {
		GF_NALUFFParamArray *pa = (GF_NALUFFParamArray*)gf_list_get(cfg->param_array, 0);
		gf_list_rem(cfg->param_array, 0);

		while (gf_list_count(pa->nalus)) {
			GF_NALUFFParam *n = (GF_NALUFFParam*)gf_list_get(pa->nalus, 0);
			gf_list_rem(pa->nalus, 0);
			if (n->data) gf_free(n->data);
			gf_free(n);
		}
		gf_list_del(pa->nalus);
		gf_free(pa);
	}
	gf_list_del(cfg->param_array);
	if (cfg->general_constraint_info)
		gf_free(cfg->general_constraint_info);
	if (cfg->sub_profiles_idc)
		gf_free(cfg->sub_profiles_idc);
	gf_free(cfg);
}