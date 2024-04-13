void kvm_mmu_slot_leaf_clear_dirty(struct kvm *kvm,
				   const struct kvm_memory_slot *memslot)
{
	bool flush = false;

	if (kvm_memslots_have_rmaps(kvm)) {
		write_lock(&kvm->mmu_lock);
		/*
		 * Clear dirty bits only on 4k SPTEs since the legacy MMU only
		 * support dirty logging at a 4k granularity.
		 */
		flush = slot_handle_level_4k(kvm, memslot, __rmap_clear_dirty, false);
		write_unlock(&kvm->mmu_lock);
	}

	if (is_tdp_mmu_enabled(kvm)) {
		read_lock(&kvm->mmu_lock);
		flush |= kvm_tdp_mmu_clear_dirty_slot(kvm, memslot);
		read_unlock(&kvm->mmu_lock);
	}

	/*
	 * It's also safe to flush TLBs out of mmu lock here as currently this
	 * function is only used for dirty logging, in which case flushing TLB
	 * out of mmu lock also guarantees no dirty pages will be lost in
	 * dirty_bitmap.
	 */
	if (flush)
		kvm_arch_flush_remote_tlbs_memslot(kvm, memslot);
}