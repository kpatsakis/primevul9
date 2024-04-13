static int FNAME(sync_page)(struct kvm_vcpu *vcpu, struct kvm_mmu_page *sp)
{
	int i, nr_present = 0;
	bool host_writable;
	gpa_t first_pte_gpa;
	int set_spte_ret = 0;

	/* direct kvm_mmu_page can not be unsync. */
	BUG_ON(sp->role.direct);

	first_pte_gpa = FNAME(get_level1_sp_gpa)(sp);

	for (i = 0; i < PT64_ENT_PER_PAGE; i++) {
		unsigned pte_access;
		pt_element_t gpte;
		gpa_t pte_gpa;
		gfn_t gfn;

		if (!sp->spt[i])
			continue;

		pte_gpa = first_pte_gpa + i * sizeof(pt_element_t);

		if (kvm_vcpu_read_guest_atomic(vcpu, pte_gpa, &gpte,
					       sizeof(pt_element_t)))
			return 0;

		if (FNAME(prefetch_invalid_gpte)(vcpu, sp, &sp->spt[i], gpte)) {
			/*
			 * Update spte before increasing tlbs_dirty to make
			 * sure no tlb flush is lost after spte is zapped; see
			 * the comments in kvm_flush_remote_tlbs().
			 */
			smp_wmb();
			vcpu->kvm->tlbs_dirty++;
			continue;
		}

		gfn = gpte_to_gfn(gpte);
		pte_access = sp->role.access;
		pte_access &= FNAME(gpte_access)(gpte);
		FNAME(protect_clean_gpte)(vcpu->arch.mmu, &pte_access, gpte);

		if (sync_mmio_spte(vcpu, &sp->spt[i], gfn, pte_access,
		      &nr_present))
			continue;

		if (gfn != sp->gfns[i]) {
			drop_spte(vcpu->kvm, &sp->spt[i]);
			/*
			 * The same as above where we are doing
			 * prefetch_invalid_gpte().
			 */
			smp_wmb();
			vcpu->kvm->tlbs_dirty++;
			continue;
		}

		nr_present++;

		host_writable = sp->spt[i] & SPTE_HOST_WRITEABLE;

		set_spte_ret |= set_spte(vcpu, &sp->spt[i],
					 pte_access, PT_PAGE_TABLE_LEVEL,
					 gfn, spte_to_pfn(sp->spt[i]),
					 true, false, host_writable);
	}

	if (set_spte_ret & SET_SPTE_NEED_REMOTE_TLB_FLUSH)
		kvm_flush_remote_tlbs(vcpu->kvm);

	return nr_present;
}