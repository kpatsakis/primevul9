static long __get_user_pages_remote(struct task_struct *tsk,
				    struct mm_struct *mm,
				    unsigned long start, unsigned long nr_pages,
				    unsigned int gup_flags, struct page **pages,
				    struct vm_area_struct **vmas, int *locked)
{
	return 0;
}