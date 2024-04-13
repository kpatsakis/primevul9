static long __get_user_pages_remote(struct task_struct *tsk,
				    struct mm_struct *mm,
				    unsigned long start, unsigned long nr_pages,
				    unsigned int gup_flags, struct page **pages,
				    struct vm_area_struct **vmas, int *locked)
{
	/*
	 * Parts of FOLL_LONGTERM behavior are incompatible with
	 * FAULT_FLAG_ALLOW_RETRY because of the FS DAX check requirement on
	 * vmas. However, this only comes up if locked is set, and there are
	 * callers that do request FOLL_LONGTERM, but do not set locked. So,
	 * allow what we can.
	 */
	if (gup_flags & FOLL_LONGTERM) {
		if (WARN_ON_ONCE(locked))
			return -EINVAL;
		/*
		 * This will check the vmas (even if our vmas arg is NULL)
		 * and return -ENOTSUPP if DAX isn't allowed in this case:
		 */
		return __gup_longterm_locked(tsk, mm, start, nr_pages, pages,
					     vmas, gup_flags | FOLL_TOUCH |
					     FOLL_REMOTE);
	}

	return __get_user_pages_locked(tsk, mm, start, nr_pages, pages, vmas,
				       locked,
				       gup_flags | FOLL_TOUCH | FOLL_REMOTE);
}