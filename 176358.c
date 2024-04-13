long get_user_pages_remote(struct task_struct *tsk, struct mm_struct *mm,
		unsigned long start, unsigned long nr_pages,
		unsigned int gup_flags, struct page **pages,
		struct vm_area_struct **vmas, int *locked)
{
	/*
	 * FOLL_PIN must only be set internally by the pin_user_pages*() APIs,
	 * never directly by the caller, so enforce that with an assertion:
	 */
	if (WARN_ON_ONCE(gup_flags & FOLL_PIN))
		return -EINVAL;

	return __get_user_pages_remote(tsk, mm, start, nr_pages, gup_flags,
				       pages, vmas, locked);
}