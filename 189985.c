static bool kvm_mmu_prepare_zap_page(struct kvm *kvm, struct kvm_mmu_page *sp,
				     struct list_head *invalid_list)
{
	int nr_zapped;

	__kvm_mmu_prepare_zap_page(kvm, sp, invalid_list, &nr_zapped);
	return nr_zapped;
}