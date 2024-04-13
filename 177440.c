static void collect_procs_anon(struct page *page, struct list_head *to_kill,
			      struct to_kill **tkc, int force_early)
{
	struct vm_area_struct *vma;
	struct task_struct *tsk;
	struct anon_vma *av;
	pgoff_t pgoff;

	av = page_lock_anon_vma_read(page);
	if (av == NULL)	/* Not actually mapped anymore */
		return;

	pgoff = page_to_pgoff(page);
	read_lock(&tasklist_lock);
	for_each_process (tsk) {
		struct anon_vma_chain *vmac;
		struct task_struct *t = task_early_kill(tsk, force_early);

		if (!t)
			continue;
		anon_vma_interval_tree_foreach(vmac, &av->rb_root,
					       pgoff, pgoff) {
			vma = vmac->vma;
			if (!page_mapped_in_vma(page, vma))
				continue;
			if (vma->vm_mm == t->mm)
				add_to_kill(t, page, vma, to_kill, tkc);
		}
	}
	read_unlock(&tasklist_lock);
	page_unlock_anon_vma_read(av);
}