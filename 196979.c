static int FNAME(fetch)(struct kvm_vcpu *vcpu, gva_t addr,
			 struct guest_walker *gw,
			 int write_fault, int hlevel,
			 kvm_pfn_t pfn, bool map_writable, bool prefault)
{
	struct kvm_mmu_page *sp = NULL;
	struct kvm_shadow_walk_iterator it;
	unsigned direct_access, access = gw->pt_access;
	int top_level, ret;

	direct_access = gw->pte_access;

	top_level = vcpu->arch.mmu->root_level;
	if (top_level == PT32E_ROOT_LEVEL)
		top_level = PT32_ROOT_LEVEL;
	/*
	 * Verify that the top-level gpte is still there.  Since the page
	 * is a root page, it is either write protected (and cannot be
	 * changed from now on) or it is invalid (in which case, we don't
	 * really care if it changes underneath us after this point).
	 */
	if (FNAME(gpte_changed)(vcpu, gw, top_level))
		goto out_gpte_changed;

	if (!VALID_PAGE(vcpu->arch.mmu->root_hpa))
		goto out_gpte_changed;

	for (shadow_walk_init(&it, vcpu, addr);
	     shadow_walk_okay(&it) && it.level > gw->level;
	     shadow_walk_next(&it)) {
		gfn_t table_gfn;

		clear_sp_write_flooding_count(it.sptep);
		drop_large_spte(vcpu, it.sptep);

		sp = NULL;
		if (!is_shadow_present_pte(*it.sptep)) {
			table_gfn = gw->table_gfn[it.level - 2];
			sp = kvm_mmu_get_page(vcpu, table_gfn, addr, it.level-1,
					      false, access);
		}

		/*
		 * Verify that the gpte in the page we've just write
		 * protected is still there.
		 */
		if (FNAME(gpte_changed)(vcpu, gw, it.level - 1))
			goto out_gpte_changed;

		if (sp)
			link_shadow_page(vcpu, it.sptep, sp);
	}

	for (;
	     shadow_walk_okay(&it) && it.level > hlevel;
	     shadow_walk_next(&it)) {
		gfn_t direct_gfn;

		clear_sp_write_flooding_count(it.sptep);
		validate_direct_spte(vcpu, it.sptep, direct_access);

		drop_large_spte(vcpu, it.sptep);

		if (is_shadow_present_pte(*it.sptep))
			continue;

		direct_gfn = gw->gfn & ~(KVM_PAGES_PER_HPAGE(it.level) - 1);

		sp = kvm_mmu_get_page(vcpu, direct_gfn, addr, it.level-1,
				      true, direct_access);
		link_shadow_page(vcpu, it.sptep, sp);
	}

	clear_sp_write_flooding_count(it.sptep);
	ret = mmu_set_spte(vcpu, it.sptep, gw->pte_access, write_fault,
			   it.level, gw->gfn, pfn, prefault, map_writable);
	FNAME(pte_prefetch)(vcpu, gw, it.sptep);

	return ret;

out_gpte_changed:
	kvm_release_pfn_clean(pfn);
	return RET_PF_RETRY;
}