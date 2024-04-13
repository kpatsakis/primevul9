static void pte_list_remove(struct kvm *kvm, struct kvm_rmap_head *rmap_head,
			    u64 *sptep)
{
	mmu_spte_clear_track_bits(kvm, sptep);
	__pte_list_remove(sptep, rmap_head);
}