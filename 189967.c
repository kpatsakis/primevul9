bool kvm_test_age_gfn(struct kvm *kvm, struct kvm_gfn_range *range)
{
	bool young = false;

	if (kvm_memslots_have_rmaps(kvm))
		young = kvm_handle_gfn_range(kvm, range, kvm_test_age_rmapp);

	if (is_tdp_mmu_enabled(kvm))
		young |= kvm_tdp_mmu_test_age_gfn(kvm, range);

	return young;
}