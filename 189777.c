char *task_mem(struct mm_struct *mm, char *buffer)
{
	unsigned long data, text, lib;
	unsigned long hiwater_vm, total_vm, hiwater_rss, total_rss;

	/*
	 * Note: to minimize their overhead, mm maintains hiwater_vm and
	 * hiwater_rss only when about to *lower* total_vm or rss.  Any
	 * collector of these hiwater stats must therefore get total_vm
	 * and rss too, which will usually be the higher.  Barriers? not
	 * worth the effort, such snapshots can always be inconsistent.
	 */
	hiwater_vm = total_vm = mm->total_vm;
	if (hiwater_vm < mm->hiwater_vm)
		hiwater_vm = mm->hiwater_vm;
	hiwater_rss = total_rss = get_mm_rss(mm);
	if (hiwater_rss < mm->hiwater_rss)
		hiwater_rss = mm->hiwater_rss;

	data = mm->total_vm - mm->shared_vm - mm->stack_vm;
	text = (PAGE_ALIGN(mm->end_code) - (mm->start_code & PAGE_MASK)) >> 10;
	lib = (mm->exec_vm << (PAGE_SHIFT-10)) - text;
	buffer += sprintf(buffer,
		"VmPeak:\t%8lu kB\n"
		"VmSize:\t%8lu kB\n"
		"VmLck:\t%8lu kB\n"
		"VmHWM:\t%8lu kB\n"
		"VmRSS:\t%8lu kB\n"
		"VmData:\t%8lu kB\n"
		"VmStk:\t%8lu kB\n"
		"VmExe:\t%8lu kB\n"
		"VmLib:\t%8lu kB\n"
		"VmPTE:\t%8lu kB\n",
		hiwater_vm << (PAGE_SHIFT-10),
		(total_vm - mm->reserved_vm) << (PAGE_SHIFT-10),
		mm->locked_vm << (PAGE_SHIFT-10),
		hiwater_rss << (PAGE_SHIFT-10),
		total_rss << (PAGE_SHIFT-10),
		data << (PAGE_SHIFT-10),
		mm->stack_vm << (PAGE_SHIFT-10), text, lib,
		(PTRS_PER_PTE*sizeof(pte_t)*mm->nr_ptes) >> 10);
	return buffer;
}