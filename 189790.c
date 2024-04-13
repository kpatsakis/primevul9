int task_statm(struct mm_struct *mm, int *shared, int *text,
	       int *data, int *resident)
{
	*shared = get_mm_counter(mm, file_rss);
	*text = (PAGE_ALIGN(mm->end_code) - (mm->start_code & PAGE_MASK))
								>> PAGE_SHIFT;
	*data = mm->total_vm - mm->shared_vm;
	*resident = *shared + get_mm_counter(mm, anon_rss);
	return mm->total_vm;
}