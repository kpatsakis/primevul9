bool kvm_set_spte_gfn(struct kvm *kvm, struct kvm_gfn_range *range)
{
	bool flush = false;

	if (kvm_memslots_have_rmaps(kvm))
		flush = kvm_handle_gfn_range(kvm, range, kvm_set_pte_rmapp);

	if (is_tdp_mmu_enabled(kvm))
		flush |= kvm_tdp_mmu_set_spte_gfn(kvm, range);

	return flush;
}