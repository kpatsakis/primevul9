void kvm_mmu_slot_remove_write_access(struct kvm *kvm,
				      const struct kvm_memory_slot *memslot,
				      int start_level)
{
	bool flush = false;

	if (kvm_memslots_have_rmaps(kvm)) {
		write_lock(&kvm->mmu_lock);
		flush = slot_handle_level(kvm, memslot, slot_rmap_write_protect,
					  start_level, KVM_MAX_HUGEPAGE_LEVEL,
					  false);
		write_unlock(&kvm->mmu_lock);
	}

	if (is_tdp_mmu_enabled(kvm)) {
		read_lock(&kvm->mmu_lock);
		flush |= kvm_tdp_mmu_wrprot_slot(kvm, memslot, start_level);
		read_unlock(&kvm->mmu_lock);
	}

	/*
	 * Flush TLBs if any SPTEs had to be write-protected to ensure that
	 * guest writes are reflected in the dirty bitmap before the memslot
	 * update completes, i.e. before enabling dirty logging is visible to
	 * userspace.
	 *
	 * Perform the TLB flush outside the mmu_lock to reduce the amount of
	 * time the lock is held. However, this does mean that another CPU can
	 * now grab mmu_lock and encounter a write-protected SPTE while CPUs
	 * still have a writable mapping for the associated GFN in their TLB.
	 *
	 * This is safe but requires KVM to be careful when making decisions
	 * based on the write-protection status of an SPTE. Specifically, KVM
	 * also write-protects SPTEs to monitor changes to guest page tables
	 * during shadow paging, and must guarantee no CPUs can write to those
	 * page before the lock is dropped. As mentioned in the previous
	 * paragraph, a write-protected SPTE is no guarantee that CPU cannot
	 * perform writes. So to determine if a TLB flush is truly required, KVM
	 * will clear a separate software-only bit (MMU-writable) and skip the
	 * flush if-and-only-if this bit was already clear.
	 *
	 * See is_writable_pte() for more details.
	 */
	if (flush)
		kvm_arch_flush_remote_tlbs_memslot(kvm, memslot);
}