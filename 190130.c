static bool spte_wrprot_for_clear_dirty(u64 *sptep)
{
	bool was_writable = test_and_clear_bit(PT_WRITABLE_SHIFT,
					       (unsigned long *)sptep);
	if (was_writable && !spte_ad_enabled(*sptep))
		kvm_set_pfn_dirty(spte_to_pfn(*sptep));

	return was_writable;
}