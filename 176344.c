long get_user_pages_locked(unsigned long start, unsigned long nr_pages,
			   unsigned int gup_flags, struct page **pages,
			   int *locked)
{
	/*
	 * FIXME: Current FOLL_LONGTERM behavior is incompatible with
	 * FAULT_FLAG_ALLOW_RETRY because of the FS DAX check requirement on
	 * vmas.  As there are no users of this flag in this call we simply
	 * disallow this option for now.
	 */
	if (WARN_ON_ONCE(gup_flags & FOLL_LONGTERM))
		return -EINVAL;

	return __get_user_pages_locked(current, current->mm, start, nr_pages,
				       pages, NULL, locked,
				       gup_flags | FOLL_TOUCH);
}