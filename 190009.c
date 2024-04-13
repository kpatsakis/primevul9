static int mmu_pages_first(struct kvm_mmu_pages *pvec,
			   struct mmu_page_path *parents)
{
	struct kvm_mmu_page *sp;
	int level;

	if (pvec->nr == 0)
		return 0;

	WARN_ON(pvec->page[0].idx != INVALID_INDEX);

	sp = pvec->page[0].sp;
	level = sp->role.level;
	WARN_ON(level == PG_LEVEL_4K);

	parents->parent[level-2] = sp;

	/* Also set up a sentinel.  Further entries in pvec are all
	 * children of sp, so this element is never overwritten.
	 */
	parents->parent[level-1] = NULL;
	return mmu_pages_next(pvec, parents, 0);
}