void kvm_mmu_invalidate_gva(struct kvm_vcpu *vcpu, struct kvm_mmu *mmu,
			    gva_t gva, hpa_t root_hpa)
{
	int i;

	/* It's actually a GPA for vcpu->arch.guest_mmu.  */
	if (mmu != &vcpu->arch.guest_mmu) {
		/* INVLPG on a non-canonical address is a NOP according to the SDM.  */
		if (is_noncanonical_address(gva, vcpu))
			return;

		static_call(kvm_x86_flush_tlb_gva)(vcpu, gva);
	}

	if (!mmu->invlpg)
		return;

	if (root_hpa == INVALID_PAGE) {
		mmu->invlpg(vcpu, gva, mmu->root.hpa);

		/*
		 * INVLPG is required to invalidate any global mappings for the VA,
		 * irrespective of PCID. Since it would take us roughly similar amount
		 * of work to determine whether any of the prev_root mappings of the VA
		 * is marked global, or to just sync it blindly, so we might as well
		 * just always sync it.
		 *
		 * Mappings not reachable via the current cr3 or the prev_roots will be
		 * synced when switching to that cr3, so nothing needs to be done here
		 * for them.
		 */
		for (i = 0; i < KVM_MMU_NUM_PREV_ROOTS; i++)
			if (VALID_PAGE(mmu->prev_roots[i].hpa))
				mmu->invlpg(vcpu, gva, mmu->prev_roots[i].hpa);
	} else {
		mmu->invlpg(vcpu, gva, root_hpa);
	}
}