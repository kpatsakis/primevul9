respip_global_apply_cfg(struct respip_set* set, struct config_file* cfg)
{
	int ret = respip_set_apply_cfg(set, cfg->tagname, cfg->num_tags,
		cfg->respip_tags, cfg->respip_actions, cfg->respip_data);
	cfg->respip_data = NULL;
	cfg->respip_actions = NULL;
	cfg->respip_tags = NULL;
	return ret;
}