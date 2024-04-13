static void collect_procs_file(struct page *page, struct list_head *to_kill,
			      struct to_kill **tkc, int force_early)
{
	struct vm_area_struct *vma;
	struct task_struct *tsk;
	struct address_space *mapping = page->mapping;

	i_mmap_lock_read(mapping);
	read_lock(&tasklist_lock);
	for_each_process(tsk) {
		pgoff_t pgoff = page_to_pgoff(page);
		struct task_struct *t = task_early_kill(tsk, force_early);

		if (!t)
			continue;
		vma_interval_tree_foreach(vma, &mapping->i_mmap, pgoff,
				      pgoff) {
			/*
			 * Send early kill signal to tasks where a vma covers
			 * the page but the corrupted page is not necessarily
			 * mapped it in its pte.
			 * Assume applications who requested early kill want
			 * to be informed of all such data corruptions.
			 */
			if (vma->vm_mm == t->mm)
				add_to_kill(t, page, vma, to_kill, tkc);
		}
	}
	read_unlock(&tasklist_lock);
	i_mmap_unlock_read(mapping);
}