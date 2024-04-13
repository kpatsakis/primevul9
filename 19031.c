respip_set_get_tree(struct respip_set* set)
{
	if(!set)
		return NULL;
	return &set->ip_tree;
}