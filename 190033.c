static struct kvm_lpage_info *lpage_info_slot(gfn_t gfn,
		const struct kvm_memory_slot *slot, int level)
{
	unsigned long idx;

	idx = gfn_to_index(gfn, slot->base_gfn, level);
	return &slot->arch.lpage_info[level - 2][idx];
}