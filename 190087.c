static __always_inline bool kvm_handle_gfn_range(struct kvm *kvm,
						 struct kvm_gfn_range *range,
						 rmap_handler_t handler)
{
	struct slot_rmap_walk_iterator iterator;
	bool ret = false;

	for_each_slot_rmap_range(range->slot, PG_LEVEL_4K, KVM_MAX_HUGEPAGE_LEVEL,
				 range->start, range->end - 1, &iterator)
		ret |= handler(kvm, iterator.rmap, range->slot, iterator.gfn,
			       iterator.level, range->pte);

	return ret;
}