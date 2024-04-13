static int __direct_map(struct kvm_vcpu *vcpu, struct kvm_page_fault *fault)
{
	struct kvm_shadow_walk_iterator it;
	struct kvm_mmu_page *sp;
	int ret;
	gfn_t base_gfn = fault->gfn;

	kvm_mmu_hugepage_adjust(vcpu, fault);

	trace_kvm_mmu_spte_requested(fault);
	for_each_shadow_entry(vcpu, fault->addr, it) {
		/*
		 * We cannot overwrite existing page tables with an NX
		 * large page, as the leaf could be executable.
		 */
		if (fault->nx_huge_page_workaround_enabled)
			disallowed_hugepage_adjust(fault, *it.sptep, it.level);

		base_gfn = fault->gfn & ~(KVM_PAGES_PER_HPAGE(it.level) - 1);
		if (it.level == fault->goal_level)
			break;

		drop_large_spte(vcpu, it.sptep);
		if (is_shadow_present_pte(*it.sptep))
			continue;

		sp = kvm_mmu_get_page(vcpu, base_gfn, it.addr,
				      it.level - 1, true, ACC_ALL);

		link_shadow_page(vcpu, it.sptep, sp);
		if (fault->is_tdp && fault->huge_page_disallowed &&
		    fault->req_level >= it.level)
			account_huge_nx_page(vcpu->kvm, sp);
	}

	if (WARN_ON_ONCE(it.level != fault->goal_level))
		return -EFAULT;

	ret = mmu_set_spte(vcpu, fault->slot, it.sptep, ACC_ALL,
			   base_gfn, fault->pfn, fault);
	if (ret == RET_PF_SPURIOUS)
		return ret;

	direct_pte_prefetch(vcpu, it.sptep);
	++vcpu->stat.pf_fixed;
	return ret;
}