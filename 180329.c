bool process_shares_mm(struct task_struct *p, struct mm_struct *mm)
{
	struct task_struct *t;

	for_each_thread(p, t) {
		struct mm_struct *t_mm = READ_ONCE(t->mm);
		if (t_mm)
			return t_mm == mm;
	}
	return false;
}