void show_stack(struct task_struct *task, unsigned long *sp)
{
	task = task ? : current;

	/*
	 * Stack frames below this one aren't interesting.  Don't show them
	 * if we're printing for %current.
	 */
	if (!sp && task == current)
		sp = get_stack_pointer(current, NULL);

	show_trace_log_lvl(task, NULL, sp, KERN_DEFAULT);
}