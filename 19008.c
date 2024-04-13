respip_set_create(void)
{
	struct respip_set* set = calloc(1, sizeof(*set));
	if(!set)
		return NULL;
	set->region = regional_create();
	if(!set->region) {
		free(set);
		return NULL;
	}
	addr_tree_init(&set->ip_tree);
	return set;
}