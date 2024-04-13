void kvm_mmu_slot_try_split_huge_pages(struct kvm *kvm,
					const struct kvm_memory_slot *memslot,
					int target_level)
{
	u64 start = memslot->base_gfn;
	u64 end = start + memslot->npages;

	if (is_tdp_mmu_enabled(kvm)) {
		read_lock(&kvm->mmu_lock);
		kvm_tdp_mmu_try_split_huge_pages(kvm, memslot, start, end, target_level, true);
		read_unlock(&kvm->mmu_lock);
	}

	/*
	 * No TLB flush is necessary here. KVM will flush TLBs after
	 * write-protecting and/or clearing dirty on the newly split SPTEs to
	 * ensure that guest writes are reflected in the dirty log before the
	 * ioctl to enable dirty logging on this memslot completes. Since the
	 * split SPTEs retain the write and dirty bits of the huge SPTE, it is
	 * safe for KVM to decide if a TLB flush is necessary based on the split
	 * SPTEs.
	 */
}