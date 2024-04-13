static int FNAME(page_fault)(struct kvm_vcpu *vcpu, gva_t addr, u32 error_code,
			     bool prefault)
{
	int write_fault = error_code & PFERR_WRITE_MASK;
	int user_fault = error_code & PFERR_USER_MASK;
	struct guest_walker walker;
	int r;
	kvm_pfn_t pfn;
	int level = PT_PAGE_TABLE_LEVEL;
	bool force_pt_level = false;
	unsigned long mmu_seq;
	bool map_writable, is_self_change_mapping;

	pgprintk("%s: addr %lx err %x\n", __func__, addr, error_code);

	r = mmu_topup_memory_caches(vcpu);
	if (r)
		return r;

	/*
	 * If PFEC.RSVD is set, this is a shadow page fault.
	 * The bit needs to be cleared before walking guest page tables.
	 */
	error_code &= ~PFERR_RSVD_MASK;

	/*
	 * Look up the guest pte for the faulting address.
	 */
	r = FNAME(walk_addr)(&walker, vcpu, addr, error_code);

	/*
	 * The page is not mapped by the guest.  Let the guest handle it.
	 */
	if (!r) {
		pgprintk("%s: guest page fault\n", __func__);
		if (!prefault)
			inject_page_fault(vcpu, &walker.fault);

		return RET_PF_RETRY;
	}

	if (page_fault_handle_page_track(vcpu, error_code, walker.gfn)) {
		shadow_page_table_clear_flood(vcpu, addr);
		return RET_PF_EMULATE;
	}

	vcpu->arch.write_fault_to_shadow_pgtable = false;

	is_self_change_mapping = FNAME(is_self_change_mapping)(vcpu,
	      &walker, user_fault, &vcpu->arch.write_fault_to_shadow_pgtable);

	if (walker.level >= PT_DIRECTORY_LEVEL && !is_self_change_mapping) {
		level = mapping_level(vcpu, walker.gfn, &force_pt_level);
		if (likely(!force_pt_level)) {
			level = min(walker.level, level);
			walker.gfn = walker.gfn & ~(KVM_PAGES_PER_HPAGE(level) - 1);
		}
	} else
		force_pt_level = true;

	mmu_seq = vcpu->kvm->mmu_notifier_seq;
	smp_rmb();

	if (try_async_pf(vcpu, prefault, walker.gfn, addr, &pfn, write_fault,
			 &map_writable))
		return RET_PF_RETRY;

	if (handle_abnormal_pfn(vcpu, addr, walker.gfn, pfn, walker.pte_access, &r))
		return r;

	/*
	 * Do not change pte_access if the pfn is a mmio page, otherwise
	 * we will cache the incorrect access into mmio spte.
	 */
	if (write_fault && !(walker.pte_access & ACC_WRITE_MASK) &&
	     !is_write_protection(vcpu) && !user_fault &&
	      !is_noslot_pfn(pfn)) {
		walker.pte_access |= ACC_WRITE_MASK;
		walker.pte_access &= ~ACC_USER_MASK;

		/*
		 * If we converted a user page to a kernel page,
		 * so that the kernel can write to it when cr0.wp=0,
		 * then we should prevent the kernel from executing it
		 * if SMEP is enabled.
		 */
		if (kvm_read_cr4_bits(vcpu, X86_CR4_SMEP))
			walker.pte_access &= ~ACC_EXEC_MASK;
	}

	spin_lock(&vcpu->kvm->mmu_lock);
	if (mmu_notifier_retry(vcpu->kvm, mmu_seq))
		goto out_unlock;

	kvm_mmu_audit(vcpu, AUDIT_PRE_PAGE_FAULT);
	if (make_mmu_pages_available(vcpu) < 0)
		goto out_unlock;
	if (!force_pt_level)
		transparent_hugepage_adjust(vcpu, &walker.gfn, &pfn, &level);
	r = FNAME(fetch)(vcpu, addr, &walker, write_fault,
			 level, pfn, map_writable, prefault);
	++vcpu->stat.pf_fixed;
	kvm_mmu_audit(vcpu, AUDIT_POST_PAGE_FAULT);
	spin_unlock(&vcpu->kvm->mmu_lock);

	return r;

out_unlock:
	spin_unlock(&vcpu->kvm->mmu_lock);
	kvm_release_pfn_clean(pfn);
	return RET_PF_RETRY;
}