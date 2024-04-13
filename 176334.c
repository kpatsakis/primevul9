int get_user_pages_fast(unsigned long start, int nr_pages,
			unsigned int gup_flags, struct page **pages)
{
	/*
	 * FOLL_PIN must only be set internally by the pin_user_pages*() APIs,
	 * never directly by the caller, so enforce that:
	 */
	if (WARN_ON_ONCE(gup_flags & FOLL_PIN))
		return -EINVAL;

	/*
	 * The caller may or may not have explicitly set FOLL_GET; either way is
	 * OK. However, internally (within mm/gup.c), gup fast variants must set
	 * FOLL_GET, because gup fast is always a "pin with a +1 page refcount"
	 * request.
	 */
	gup_flags |= FOLL_GET;
	return internal_get_user_pages_fast(start, nr_pages, gup_flags, pages);
}