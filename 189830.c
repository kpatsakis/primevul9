unsigned long task_vsize(struct mm_struct *mm)
{
	return PAGE_SIZE * mm->total_vm;
}