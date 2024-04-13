static void mmu_free_root_page(struct kvm *kvm, hpa_t *root_hpa,
			       struct list_head *invalid_list)
{
	struct kvm_mmu_page *sp;

	if (!VALID_PAGE(*root_hpa))
		return;

	sp = to_shadow_page(*root_hpa & PT64_BASE_ADDR_MASK);
	if (WARN_ON(!sp))
		return;

	if (is_tdp_mmu_page(sp))
		kvm_tdp_mmu_put_root(kvm, sp, false);
	else if (!--sp->root_count && sp->role.invalid)
		kvm_mmu_prepare_zap_page(kvm, sp, invalid_list);

	*root_hpa = INVALID_PAGE;
}