static ssize_t pagemap_read(struct file *file, char __user *buf,
			    size_t count, loff_t *ppos)
{
	struct task_struct *task = get_proc_task(file->f_path.dentry->d_inode);
	struct page **pages, *page;
	unsigned long uaddr, uend;
	struct mm_struct *mm;
	struct pagemapread pm;
	int pagecount;
	int ret = -ESRCH;

	if (!task)
		goto out;

	ret = -EACCES;
	if (!ptrace_may_attach(task))
		goto out;

	ret = -EINVAL;
	/* file position must be aligned */
	if (*ppos % PM_ENTRY_BYTES)
		goto out;

	ret = 0;
	mm = get_task_mm(task);
	if (!mm)
		goto out;

	ret = -ENOMEM;
	uaddr = (unsigned long)buf & PAGE_MASK;
	uend = (unsigned long)(buf + count);
	pagecount = (PAGE_ALIGN(uend) - uaddr) / PAGE_SIZE;
	pages = kmalloc(pagecount * sizeof(struct page *), GFP_KERNEL);
	if (!pages)
		goto out_task;

	down_read(&current->mm->mmap_sem);
	ret = get_user_pages(current, current->mm, uaddr, pagecount,
			     1, 0, pages, NULL);
	up_read(&current->mm->mmap_sem);

	if (ret < 0)
		goto out_free;

	pm.out = buf;
	pm.end = buf + count;

	if (!ptrace_may_attach(task)) {
		ret = -EIO;
	} else {
		unsigned long src = *ppos;
		unsigned long svpfn = src / PM_ENTRY_BYTES;
		unsigned long start_vaddr = svpfn << PAGE_SHIFT;
		unsigned long end_vaddr = TASK_SIZE_OF(task);

		/* watch out for wraparound */
		if (svpfn > TASK_SIZE_OF(task) >> PAGE_SHIFT)
			start_vaddr = end_vaddr;

		/*
		 * The odds are that this will stop walking way
		 * before end_vaddr, because the length of the
		 * user buffer is tracked in "pm", and the walk
		 * will stop when we hit the end of the buffer.
		 */
		ret = walk_page_range(mm, start_vaddr, end_vaddr,
					&pagemap_walk, &pm);
		if (ret == PM_END_OF_BUFFER)
			ret = 0;
		/* don't need mmap_sem for these, but this looks cleaner */
		*ppos += pm.out - buf;
		if (!ret)
			ret = pm.out - buf;
	}

	for (; pagecount; pagecount--) {
		page = pages[pagecount-1];
		if (!PageReserved(page))
			SetPageDirty(page);
		page_cache_release(page);
	}
	mmput(mm);
out_free:
	kfree(pages);
out_task:
	put_task_struct(task);
out:
	return ret;
}