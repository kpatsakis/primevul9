static bool cec_add_mce(struct mce *m)
{
	if (!m)
		return false;

	/* We eat only correctable DRAM errors with usable addresses. */
	if (mce_is_memory_error(m) &&
	    mce_is_correctable(m)  &&
	    mce_usable_address(m))
		if (!cec_add_elem(m->addr >> PAGE_SHIFT))
			return true;

	return false;
}