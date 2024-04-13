static struct kvm_rmap_head *gfn_to_rmap(gfn_t gfn, int level,
					 const struct kvm_memory_slot *slot)
{
	unsigned long idx;

	idx = gfn_to_index(gfn, slot->base_gfn, level);
	return &slot->arch.rmap[level - PG_LEVEL_4K][idx];
}