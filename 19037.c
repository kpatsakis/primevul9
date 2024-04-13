respip_set_is_empty(const struct respip_set* set)
{
	return set ? set->ip_tree.count == 0 : 1;
}