long get_user_pages_unlocked(unsigned long start, unsigned long nr_pages,
			     struct page **pages, unsigned int gup_flags)
{
	struct mm_struct *mm = current->mm;
	int locked = 1;
	long ret;

	/*
	 * FIXME: Current FOLL_LONGTERM behavior is incompatible with
	 * FAULT_FLAG_ALLOW_RETRY because of the FS DAX check requirement on
	 * vmas.  As there are no users of this flag in this call we simply
	 * disallow this option for now.
	 */
	if (WARN_ON_ONCE(gup_flags & FOLL_LONGTERM))
		return -EINVAL;

	down_read(&mm->mmap_sem);
	ret = __get_user_pages_locked(current, mm, start, nr_pages, pages, NULL,
				      &locked, gup_flags | FOLL_TOUCH);
	if (locked)
		up_read(&mm->mmap_sem);
	return ret;
}