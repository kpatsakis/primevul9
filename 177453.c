static struct task_struct *find_early_kill_thread(struct task_struct *tsk)
{
	struct task_struct *t;

	for_each_thread(tsk, t)
		if ((t->flags & PF_MCE_PROCESS) && (t->flags & PF_MCE_EARLY))
			return t;
	return NULL;
}