static int fast_page_fault(struct kvm_vcpu *vcpu, struct kvm_page_fault *fault)
{
	struct kvm_mmu_page *sp;
	int ret = RET_PF_INVALID;
	u64 spte = 0ull;
	u64 *sptep = NULL;
	uint retry_count = 0;

	if (!page_fault_can_be_fast(fault))
		return ret;

	walk_shadow_page_lockless_begin(vcpu);

	do {
		u64 new_spte;

		if (is_tdp_mmu(vcpu->arch.mmu))
			sptep = kvm_tdp_mmu_fast_pf_get_last_sptep(vcpu, fault->addr, &spte);
		else
			sptep = fast_pf_get_last_sptep(vcpu, fault->addr, &spte);

		if (!is_shadow_present_pte(spte))
			break;

		sp = sptep_to_sp(sptep);
		if (!is_last_spte(spte, sp->role.level))
			break;

		/*
		 * Check whether the memory access that caused the fault would
		 * still cause it if it were to be performed right now. If not,
		 * then this is a spurious fault caused by TLB lazily flushed,
		 * or some other CPU has already fixed the PTE after the
		 * current CPU took the fault.
		 *
		 * Need not check the access of upper level table entries since
		 * they are always ACC_ALL.
		 */
		if (is_access_allowed(fault, spte)) {
			ret = RET_PF_SPURIOUS;
			break;
		}

		new_spte = spte;

		if (is_access_track_spte(spte))
			new_spte = restore_acc_track_spte(new_spte);

		/*
		 * Currently, to simplify the code, write-protection can
		 * be removed in the fast path only if the SPTE was
		 * write-protected for dirty-logging or access tracking.
		 */
		if (fault->write && is_mmu_writable_spte(spte)) {
			new_spte |= PT_WRITABLE_MASK;

			/*
			 * Do not fix write-permission on the large spte when
			 * dirty logging is enabled. Since we only dirty the
			 * first page into the dirty-bitmap in
			 * fast_pf_fix_direct_spte(), other pages are missed
			 * if its slot has dirty logging enabled.
			 *
			 * Instead, we let the slow page fault path create a
			 * normal spte to fix the access.
			 */
			if (sp->role.level > PG_LEVEL_4K &&
			    kvm_slot_dirty_track_enabled(fault->slot))
				break;
		}

		/* Verify that the fault can be handled in the fast path */
		if (new_spte == spte ||
		    !is_access_allowed(fault, new_spte))
			break;

		/*
		 * Currently, fast page fault only works for direct mapping
		 * since the gfn is not stable for indirect shadow page. See
		 * Documentation/virt/kvm/locking.rst to get more detail.
		 */
		if (fast_pf_fix_direct_spte(vcpu, fault, sptep, spte, new_spte)) {
			ret = RET_PF_FIXED;
			break;
		}

		if (++retry_count > 4) {
			printk_once(KERN_WARNING
				"kvm: Fast #PF retrying more than 4 times.\n");
			break;
		}

	} while (true);

	trace_fast_page_fault(vcpu, fault, sptep, spte, ret);
	walk_shadow_page_lockless_end(vcpu);

	return ret;
}