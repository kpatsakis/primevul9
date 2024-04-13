static void mark_oom_victim(struct task_struct *tsk)
{
	struct mm_struct *mm = tsk->mm;

	WARN_ON(oom_killer_disabled);
	/* OOM killer might race with memcg OOM */
	if (test_and_set_tsk_thread_flag(tsk, TIF_MEMDIE))
		return;

	/* oom_mm is bound to the signal struct life time. */
	if (!cmpxchg(&tsk->signal->oom_mm, NULL, mm))
		mmgrab(tsk->signal->oom_mm);

	/*
	 * Make sure that the task is woken up from uninterruptible sleep
	 * if it is frozen because OOM killer wouldn't be able to free
	 * any memory and livelock. freezing_slow_path will tell the freezer
	 * that TIF_MEMDIE tasks should be ignored.
	 */
	__thaw_task(tsk);
	atomic_inc(&oom_victims);
	trace_mark_victim(tsk->pid);
}