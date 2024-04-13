static void kvm_mmu_mark_parents_unsync(struct kvm_mmu_page *sp)
{
	u64 *sptep;
	struct rmap_iterator iter;

	for_each_rmap_spte(&sp->parent_ptes, &iter, sptep) {
		mark_unsync(sptep);
	}
}