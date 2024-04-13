static int kvm_mmu_page_unlink_children(struct kvm *kvm,
					struct kvm_mmu_page *sp,
					struct list_head *invalid_list)
{
	int zapped = 0;
	unsigned i;

	for (i = 0; i < PT64_ENT_PER_PAGE; ++i)
		zapped += mmu_page_zap_pte(kvm, sp, sp->spt + i, invalid_list);

	return zapped;
}