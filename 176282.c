long pin_user_pages(unsigned long start, unsigned long nr_pages,
		    unsigned int gup_flags, struct page **pages,
		    struct vm_area_struct **vmas)
{
	/* FOLL_GET and FOLL_PIN are mutually exclusive. */
	if (WARN_ON_ONCE(gup_flags & FOLL_GET))
		return -EINVAL;

	gup_flags |= FOLL_PIN;
	return __gup_longterm_locked(current, current->mm, start, nr_pages,
				     pages, vmas, gup_flags);
}