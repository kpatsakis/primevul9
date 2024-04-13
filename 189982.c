static void kvm_mmu_clear_dirty_pt_masked(struct kvm *kvm,
					 struct kvm_memory_slot *slot,
					 gfn_t gfn_offset, unsigned long mask)
{
	struct kvm_rmap_head *rmap_head;

	if (is_tdp_mmu_enabled(kvm))
		kvm_tdp_mmu_clear_dirty_pt_masked(kvm, slot,
				slot->base_gfn + gfn_offset, mask, false);

	if (!kvm_memslots_have_rmaps(kvm))
		return;

	while (mask) {
		rmap_head = gfn_to_rmap(slot->base_gfn + gfn_offset + __ffs(mask),
					PG_LEVEL_4K, slot);
		__rmap_clear_dirty(kvm, rmap_head, slot);

		/* clear the first set bit */
		mask &= mask - 1;
	}
}