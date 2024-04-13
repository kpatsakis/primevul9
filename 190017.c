slot_handle_level_range(struct kvm *kvm, const struct kvm_memory_slot *memslot,
			slot_level_handler fn, int start_level, int end_level,
			gfn_t start_gfn, gfn_t end_gfn, bool flush_on_yield,
			bool flush)
{
	struct slot_rmap_walk_iterator iterator;

	for_each_slot_rmap_range(memslot, start_level, end_level, start_gfn,
			end_gfn, &iterator) {
		if (iterator.rmap)
			flush |= fn(kvm, iterator.rmap, memslot);

		if (need_resched() || rwlock_needbreak(&kvm->mmu_lock)) {
			if (flush && flush_on_yield) {
				kvm_flush_remote_tlbs_with_address(kvm,
						start_gfn,
						iterator.gfn - start_gfn + 1);
				flush = false;
			}
			cond_resched_rwlock_write(&kvm->mmu_lock);
		}
	}

	return flush;
}