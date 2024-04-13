long get_user_pages(unsigned long start, unsigned long nr_pages,
		unsigned int gup_flags, struct page **pages,
		struct vm_area_struct **vmas)
{
	/*
	 * FOLL_PIN must only be set internally by the pin_user_pages*() APIs,
	 * never directly by the caller, so enforce that with an assertion:
	 */
	if (WARN_ON_ONCE(gup_flags & FOLL_PIN))
		return -EINVAL;

	return __gup_longterm_locked(current, current->mm, start, nr_pages,
				     pages, vmas, gup_flags | FOLL_TOUCH);
}