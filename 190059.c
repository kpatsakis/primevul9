fast_pf_fix_direct_spte(struct kvm_vcpu *vcpu, struct kvm_page_fault *fault,
			u64 *sptep, u64 old_spte, u64 new_spte)
{
	/*
	 * Theoretically we could also set dirty bit (and flush TLB) here in
	 * order to eliminate unnecessary PML logging. See comments in
	 * set_spte. But fast_page_fault is very unlikely to happen with PML
	 * enabled, so we do not do this. This might result in the same GPA
	 * to be logged in PML buffer again when the write really happens, and
	 * eventually to be called by mark_page_dirty twice. But it's also no
	 * harm. This also avoids the TLB flush needed after setting dirty bit
	 * so non-PML cases won't be impacted.
	 *
	 * Compare with set_spte where instead shadow_dirty_mask is set.
	 */
	if (cmpxchg64(sptep, old_spte, new_spte) != old_spte)
		return false;

	if (is_writable_pte(new_spte) && !is_writable_pte(old_spte))
		mark_page_dirty_in_slot(vcpu->kvm, fault->slot, fault->gfn);

	return true;
}