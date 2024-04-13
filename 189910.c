void kvm_arch_mmu_enable_log_dirty_pt_masked(struct kvm *kvm,
				struct kvm_memory_slot *slot,
				gfn_t gfn_offset, unsigned long mask)
{
	/*
	 * Huge pages are NOT write protected when we start dirty logging in
	 * initially-all-set mode; must write protect them here so that they
	 * are split to 4K on the first write.
	 *
	 * The gfn_offset is guaranteed to be aligned to 64, but the base_gfn
	 * of memslot has no such restriction, so the range can cross two large
	 * pages.
	 */
	if (kvm_dirty_log_manual_protect_and_init_set(kvm)) {
		gfn_t start = slot->base_gfn + gfn_offset + __ffs(mask);
		gfn_t end = slot->base_gfn + gfn_offset + __fls(mask);

		if (READ_ONCE(eager_page_split))
			kvm_mmu_try_split_huge_pages(kvm, slot, start, end, PG_LEVEL_4K);

		kvm_mmu_slot_gfn_write_protect(kvm, slot, start, PG_LEVEL_2M);

		/* Cross two large pages? */
		if (ALIGN(start << PAGE_SHIFT, PMD_SIZE) !=
		    ALIGN(end << PAGE_SHIFT, PMD_SIZE))
			kvm_mmu_slot_gfn_write_protect(kvm, slot, end,
						       PG_LEVEL_2M);
	}

	/* Now handle 4K PTEs.  */
	if (kvm_x86_ops.cpu_dirty_log_size)
		kvm_mmu_clear_dirty_pt_masked(kvm, slot, gfn_offset, mask);
	else
		kvm_mmu_write_protect_pt_masked(kvm, slot, gfn_offset, mask);
}