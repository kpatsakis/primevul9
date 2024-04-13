void kvm_mmu_try_split_huge_pages(struct kvm *kvm,
				   const struct kvm_memory_slot *memslot,
				   u64 start, u64 end,
				   int target_level)
{
	if (is_tdp_mmu_enabled(kvm))
		kvm_tdp_mmu_try_split_huge_pages(kvm, memslot, start, end,
						 target_level, false);

	/*
	 * A TLB flush is unnecessary at this point for the same resons as in
	 * kvm_mmu_slot_try_split_huge_pages().
	 */
}