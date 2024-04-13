static int add_to_pagemap(unsigned long addr, u64 pfn,
			  struct pagemapread *pm)
{
	/*
	 * Make sure there's room in the buffer for an
	 * entire entry.  Otherwise, only copy part of
	 * the pfn.
	 */
	if (pm->out + PM_ENTRY_BYTES >= pm->end) {
		if (copy_to_user(pm->out, &pfn, pm->end - pm->out))
			return -EFAULT;
		pm->out = pm->end;
		return PM_END_OF_BUFFER;
	}

	if (put_user(pfn, pm->out))
		return -EFAULT;
	pm->out += PM_ENTRY_BYTES;
	return 0;
}