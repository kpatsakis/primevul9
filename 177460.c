static struct task_struct *task_early_kill(struct task_struct *tsk,
					   int force_early)
{
	struct task_struct *t;
	if (!tsk->mm)
		return NULL;
	if (force_early)
		return tsk;
	t = find_early_kill_thread(tsk);
	if (t)
		return t;
	if (sysctl_memory_failure_early_kill)
		return tsk;
	return NULL;
}