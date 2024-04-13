static void FNAME(invlpg)(struct kvm_vcpu *vcpu, gva_t gva, hpa_t root_hpa)
{
	struct kvm_shadow_walk_iterator iterator;
	struct kvm_mmu_page *sp;
	int level;
	u64 *sptep;

	vcpu_clear_mmio_info(vcpu, gva);

	/*
	 * No need to check return value here, rmap_can_add() can
	 * help us to skip pte prefetch later.
	 */
	mmu_topup_memory_caches(vcpu);

	if (!VALID_PAGE(root_hpa)) {
		WARN_ON(1);
		return;
	}

	spin_lock(&vcpu->kvm->mmu_lock);
	for_each_shadow_entry_using_root(vcpu, root_hpa, gva, iterator) {
		level = iterator.level;
		sptep = iterator.sptep;

		sp = page_header(__pa(sptep));
		if (is_last_spte(*sptep, level)) {
			pt_element_t gpte;
			gpa_t pte_gpa;

			if (!sp->unsync)
				break;

			pte_gpa = FNAME(get_level1_sp_gpa)(sp);
			pte_gpa += (sptep - sp->spt) * sizeof(pt_element_t);

			if (mmu_page_zap_pte(vcpu->kvm, sp, sptep))
				kvm_flush_remote_tlbs_with_address(vcpu->kvm,
					sp->gfn, KVM_PAGES_PER_HPAGE(sp->role.level));

			if (!rmap_can_add(vcpu))
				break;

			if (kvm_vcpu_read_guest_atomic(vcpu, pte_gpa, &gpte,
						       sizeof(pt_element_t)))
				break;

			FNAME(update_pte)(vcpu, sp, sptep, &gpte);
		}

		if (!is_shadow_present_pte(*sptep) || !sp->unsync_children)
			break;
	}
	spin_unlock(&vcpu->kvm->mmu_lock);
}