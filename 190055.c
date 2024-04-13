static int mmu_set_spte(struct kvm_vcpu *vcpu, struct kvm_memory_slot *slot,
			u64 *sptep, unsigned int pte_access, gfn_t gfn,
			kvm_pfn_t pfn, struct kvm_page_fault *fault)
{
	struct kvm_mmu_page *sp = sptep_to_sp(sptep);
	int level = sp->role.level;
	int was_rmapped = 0;
	int ret = RET_PF_FIXED;
	bool flush = false;
	bool wrprot;
	u64 spte;

	/* Prefetching always gets a writable pfn.  */
	bool host_writable = !fault || fault->map_writable;
	bool prefetch = !fault || fault->prefetch;
	bool write_fault = fault && fault->write;

	pgprintk("%s: spte %llx write_fault %d gfn %llx\n", __func__,
		 *sptep, write_fault, gfn);

	if (unlikely(is_noslot_pfn(pfn))) {
		mark_mmio_spte(vcpu, sptep, gfn, pte_access);
		return RET_PF_EMULATE;
	}

	if (is_shadow_present_pte(*sptep)) {
		/*
		 * If we overwrite a PTE page pointer with a 2MB PMD, unlink
		 * the parent of the now unreachable PTE.
		 */
		if (level > PG_LEVEL_4K && !is_large_pte(*sptep)) {
			struct kvm_mmu_page *child;
			u64 pte = *sptep;

			child = to_shadow_page(pte & PT64_BASE_ADDR_MASK);
			drop_parent_pte(child, sptep);
			flush = true;
		} else if (pfn != spte_to_pfn(*sptep)) {
			pgprintk("hfn old %llx new %llx\n",
				 spte_to_pfn(*sptep), pfn);
			drop_spte(vcpu->kvm, sptep);
			flush = true;
		} else
			was_rmapped = 1;
	}

	wrprot = make_spte(vcpu, sp, slot, pte_access, gfn, pfn, *sptep, prefetch,
			   true, host_writable, &spte);

	if (*sptep == spte) {
		ret = RET_PF_SPURIOUS;
	} else {
		flush |= mmu_spte_update(sptep, spte);
		trace_kvm_mmu_set_spte(level, gfn, sptep);
	}

	if (wrprot) {
		if (write_fault)
			ret = RET_PF_EMULATE;
	}

	if (flush)
		kvm_flush_remote_tlbs_with_address(vcpu->kvm, gfn,
				KVM_PAGES_PER_HPAGE(level));

	pgprintk("%s: setting spte %llx\n", __func__, *sptep);

	if (!was_rmapped) {
		WARN_ON_ONCE(ret == RET_PF_SPURIOUS);
		kvm_update_page_stats(vcpu->kvm, level, 1);
		rmap_add(vcpu, slot, sptep, gfn);
	}

	return ret;
}