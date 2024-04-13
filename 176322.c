long pin_user_pages_remote(struct task_struct *tsk, struct mm_struct *mm,
			   unsigned long start, unsigned long nr_pages,
			   unsigned int gup_flags, struct page **pages,
			   struct vm_area_struct **vmas, int *locked)
{
	/* FOLL_GET and FOLL_PIN are mutually exclusive. */
	if (WARN_ON_ONCE(gup_flags & FOLL_GET))
		return -EINVAL;

	gup_flags |= FOLL_PIN;
	return __get_user_pages_remote(tsk, mm, start, nr_pages, gup_flags,
				       pages, vmas, locked);
}