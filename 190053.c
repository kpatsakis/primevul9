static void drop_spte(struct kvm *kvm, u64 *sptep)
{
	u64 old_spte = mmu_spte_clear_track_bits(kvm, sptep);

	if (is_shadow_present_pte(old_spte))
		rmap_remove(kvm, sptep);
}