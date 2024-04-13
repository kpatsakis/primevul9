respip_set_apply_cfg(struct respip_set* set, char* const* tagname, int num_tags,
	struct config_strbytelist* respip_tags,
	struct config_str2list* respip_actions,
	struct config_str2list* respip_data)
{
	struct config_strbytelist* p;
	struct config_str2list* pa;
	struct config_str2list* pd;

	set->tagname = tagname;
	set->num_tags = num_tags;

	p = respip_tags;
	while(p) {
		struct config_strbytelist* np = p->next;

		log_assert(p->str && p->str2);
		if(!respip_tag_cfg(set, p->str, p->str2, p->str2len)) {
			config_del_strbytelist(p);
			return 0;
		}
		free(p->str);
		free(p->str2);
		free(p);
		p = np;
	}

	pa = respip_actions;
	while(pa) {
		struct config_str2list* np = pa->next;
		log_assert(pa->str && pa->str2);
		if(!respip_action_cfg(set, pa->str, pa->str2)) {
			config_deldblstrlist(pa);
			return 0;
		}
		free(pa->str);
		free(pa->str2);
		free(pa);
		pa = np;
	}

	pd = respip_data;
	while(pd) {
		struct config_str2list* np = pd->next;
		log_assert(pd->str && pd->str2);
		if(!respip_data_cfg(set, pd->str, pd->str2)) {
			config_deldblstrlist(pd);
			return 0;
		}
		free(pd->str);
		free(pd->str2);
		free(pd);
		pd = np;
	}
	addr_tree_init_parents(&set->ip_tree);

	return 1;
}