static void mmu_pages_clear_parents(struct mmu_page_path *parents)
{
	struct kvm_mmu_page *sp;
	unsigned int level = 0;

	do {
		unsigned int idx = parents->idx[level];
		sp = parents->parent[level];
		if (!sp)
			return;

		WARN_ON(idx == INVALID_INDEX);
		clear_unsync_child_bit(sp, idx);
		level++;
	} while (!sp->unsync_children);
}