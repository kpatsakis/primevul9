static __always_inline long __gup_longterm_locked(struct task_struct *tsk,
						  struct mm_struct *mm,
						  unsigned long start,
						  unsigned long nr_pages,
						  struct page **pages,
						  struct vm_area_struct **vmas,
						  unsigned int flags)
{
	return __get_user_pages_locked(tsk, mm, start, nr_pages, pages, vmas,
				       NULL, flags);
}