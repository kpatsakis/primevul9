static int mmu_spte_clear_track_bits(struct kvm *kvm, u64 *sptep)
{
	kvm_pfn_t pfn;
	u64 old_spte = *sptep;
	int level = sptep_to_sp(sptep)->role.level;

	if (!is_shadow_present_pte(old_spte) ||
	    !spte_has_volatile_bits(old_spte))
		__update_clear_spte_fast(sptep, 0ull);
	else
		old_spte = __update_clear_spte_slow(sptep, 0ull);

	if (!is_shadow_present_pte(old_spte))
		return old_spte;

	kvm_update_page_stats(kvm, level, -1);

	pfn = spte_to_pfn(old_spte);

	/*
	 * KVM does not hold the refcount of the page used by
	 * kvm mmu, before reclaiming the page, we should
	 * unmap it from mmu first.
	 */
	WARN_ON(!kvm_is_reserved_pfn(pfn) && !page_count(pfn_to_page(pfn)));

	if (is_accessed_spte(old_spte))
		kvm_set_pfn_accessed(pfn);

	if (is_dirty_spte(old_spte))
		kvm_set_pfn_dirty(pfn);

	return old_spte;
}