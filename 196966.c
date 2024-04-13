FNAME(prefetch_gpte)(struct kvm_vcpu *vcpu, struct kvm_mmu_page *sp,
		     u64 *spte, pt_element_t gpte, bool no_dirty_log)
{
	unsigned pte_access;
	gfn_t gfn;
	kvm_pfn_t pfn;

	if (FNAME(prefetch_invalid_gpte)(vcpu, sp, spte, gpte))
		return false;

	pgprintk("%s: gpte %llx spte %p\n", __func__, (u64)gpte, spte);

	gfn = gpte_to_gfn(gpte);
	pte_access = sp->role.access & FNAME(gpte_access)(gpte);
	FNAME(protect_clean_gpte)(vcpu->arch.mmu, &pte_access, gpte);
	pfn = pte_prefetch_gfn_to_pfn(vcpu, gfn,
			no_dirty_log && (pte_access & ACC_WRITE_MASK));
	if (is_error_pfn(pfn))
		return false;

	/*
	 * we call mmu_set_spte() with host_writable = true because
	 * pte_prefetch_gfn_to_pfn always gets a writable pfn.
	 */
	mmu_set_spte(vcpu, spte, pte_access, 0, PT_PAGE_TABLE_LEVEL, gfn, pfn,
		     true, true);

	return true;
}