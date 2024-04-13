static int mmu_page_zap_pte(struct kvm *kvm, struct kvm_mmu_page *sp,
			    u64 *spte, struct list_head *invalid_list)
{
	u64 pte;
	struct kvm_mmu_page *child;

	pte = *spte;
	if (is_shadow_present_pte(pte)) {
		if (is_last_spte(pte, sp->role.level)) {
			drop_spte(kvm, spte);
		} else {
			child = to_shadow_page(pte & PT64_BASE_ADDR_MASK);
			drop_parent_pte(child, spte);

			/*
			 * Recursively zap nested TDP SPs, parentless SPs are
			 * unlikely to be used again in the near future.  This
			 * avoids retaining a large number of stale nested SPs.
			 */
			if (tdp_enabled && invalid_list &&
			    child->role.guest_mode && !child->parent_ptes.val)
				return kvm_mmu_prepare_zap_page(kvm, child,
								invalid_list);
		}
	} else if (is_mmio_spte(pte)) {
		mmu_spte_clear_no_track(spte);
	}
	return 0;
}