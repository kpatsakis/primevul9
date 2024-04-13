static bool __kvm_zap_rmaps(struct kvm *kvm, gfn_t gfn_start, gfn_t gfn_end)
{
	const struct kvm_memory_slot *memslot;
	struct kvm_memslots *slots;
	struct kvm_memslot_iter iter;
	bool flush = false;
	gfn_t start, end;
	int i;

	if (!kvm_memslots_have_rmaps(kvm))
		return flush;

	for (i = 0; i < KVM_ADDRESS_SPACE_NUM; i++) {
		slots = __kvm_memslots(kvm, i);

		kvm_for_each_memslot_in_gfn_range(&iter, slots, gfn_start, gfn_end) {
			memslot = iter.slot;
			start = max(gfn_start, memslot->base_gfn);
			end = min(gfn_end, memslot->base_gfn + memslot->npages);
			if (WARN_ON_ONCE(start >= end))
				continue;

			flush = slot_handle_level_range(kvm, memslot, kvm_zap_rmapp,

							PG_LEVEL_4K, KVM_MAX_HUGEPAGE_LEVEL,
							start, end - 1, true, flush);
		}
	}

	return flush;
}