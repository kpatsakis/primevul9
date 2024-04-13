respip_set_delete(struct respip_set* set)
{
	if(!set)
		return;
	regional_destroy(set->region);
	free(set);
}