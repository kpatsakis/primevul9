static bool shadow_walk_okay(struct kvm_shadow_walk_iterator *iterator)
{
	if (iterator->level < PG_LEVEL_4K)
		return false;

	iterator->index = SHADOW_PT_INDEX(iterator->addr, iterator->level);
	iterator->sptep	= ((u64 *)__va(iterator->shadow_addr)) + iterator->index;
	return true;
}