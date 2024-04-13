static int get_nr_threads(struct task_struct *tsk)
{
	/* Must be called with the rcu_read_lock held */
	unsigned long flags;
	int count = 0;

	if (lock_task_sighand(tsk, &flags)) {
		count = atomic_read(&tsk->signal->count);
		unlock_task_sighand(tsk, &flags);
	}
	return count;
}