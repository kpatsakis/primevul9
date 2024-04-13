static void wake_oom_reaper(struct task_struct *tsk)
{
	if (!oom_reaper_th)
		return;

	/* tsk is already queued? */
	if (tsk == oom_reaper_list || tsk->oom_reaper_list)
		return;

	get_task_struct(tsk);

	spin_lock(&oom_reaper_lock);
	tsk->oom_reaper_list = oom_reaper_list;
	oom_reaper_list = tsk;
	spin_unlock(&oom_reaper_lock);
	trace_wake_reaper(tsk->pid);
	wake_up(&oom_reaper_wait);
}