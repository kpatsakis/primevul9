static inline bool __task_will_free_mem(struct task_struct *task)
{
	struct signal_struct *sig = task->signal;

	/*
	 * A coredumping process may sleep for an extended period in exit_mm(),
	 * so the oom killer cannot assume that the process will promptly exit
	 * and release memory.
	 */
	if (sig->flags & SIGNAL_GROUP_COREDUMP)
		return false;

	if (sig->flags & SIGNAL_GROUP_EXIT)
		return true;

	if (thread_group_empty(task) && (task->flags & PF_EXITING))
		return true;

	return false;
}