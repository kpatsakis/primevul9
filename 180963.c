static bool oom_reserves_allowed(struct task_struct *tsk)
{
	if (!tsk_is_oom_victim(tsk))
		return false;

	/*
	 * !MMU doesn't have oom reaper so give access to memory reserves
	 * only to the thread with TIF_MEMDIE set
	 */
	if (!IS_ENABLED(CONFIG_MMU) && !test_thread_flag(TIF_MEMDIE))
		return false;

	return true;
}