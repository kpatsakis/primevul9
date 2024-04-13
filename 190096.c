static void kvm_mmu_unlink_parents(struct kvm_mmu_page *sp)
{
	u64 *sptep;
	struct rmap_iterator iter;

	while ((sptep = rmap_get_first(&sp->parent_ptes, &iter)))
		drop_parent_pte(sp, sptep);
}