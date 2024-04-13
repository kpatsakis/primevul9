void account_huge_nx_page(struct kvm *kvm, struct kvm_mmu_page *sp)
{
	if (sp->lpage_disallowed)
		return;

	++kvm->stat.nx_lpage_splits;
	list_add_tail(&sp->lpage_disallowed_link,
		      &kvm->arch.lpage_disallowed_mmu_pages);
	sp->lpage_disallowed = true;
}