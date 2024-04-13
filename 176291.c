static long __gup_longterm_locked(struct task_struct *tsk,
				  struct mm_struct *mm,
				  unsigned long start,
				  unsigned long nr_pages,
				  struct page **pages,
				  struct vm_area_struct **vmas,
				  unsigned int gup_flags)
{
	struct vm_area_struct **vmas_tmp = vmas;
	unsigned long flags = 0;
	long rc, i;

	if (gup_flags & FOLL_LONGTERM) {
		if (!pages)
			return -EINVAL;

		if (!vmas_tmp) {
			vmas_tmp = kcalloc(nr_pages,
					   sizeof(struct vm_area_struct *),
					   GFP_KERNEL);
			if (!vmas_tmp)
				return -ENOMEM;
		}
		flags = memalloc_nocma_save();
	}

	rc = __get_user_pages_locked(tsk, mm, start, nr_pages, pages,
				     vmas_tmp, NULL, gup_flags);

	if (gup_flags & FOLL_LONGTERM) {
		memalloc_nocma_restore(flags);
		if (rc < 0)
			goto out;

		if (check_dax_vmas(vmas_tmp, rc)) {
			for (i = 0; i < rc; i++)
				put_page(pages[i]);
			rc = -EOPNOTSUPP;
			goto out;
		}

		rc = check_and_migrate_cma_pages(tsk, mm, start, rc, pages,
						 vmas_tmp, gup_flags);
	}

out:
	if (vmas_tmp != vmas)
		kfree(vmas_tmp);
	return rc;
}