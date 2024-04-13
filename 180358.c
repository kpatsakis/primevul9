static int oom_reaper(void *unused)
{
	while (true) {
		struct task_struct *tsk = NULL;

		wait_event_freezable(oom_reaper_wait, oom_reaper_list != NULL);
		spin_lock(&oom_reaper_lock);
		if (oom_reaper_list != NULL) {
			tsk = oom_reaper_list;
			oom_reaper_list = tsk->oom_reaper_list;
		}
		spin_unlock(&oom_reaper_lock);

		if (tsk)
			oom_reap_task(tsk);
	}

	return 0;
}