int posix_timer_event(struct k_itimer *timr, int si_private)
{
	struct task_struct *task;
	int shared, ret = -1;
	/*
	 * FIXME: if ->sigq is queued we can race with
	 * dequeue_signal()->posixtimer_rearm().
	 *
	 * If dequeue_signal() sees the "right" value of
	 * si_sys_private it calls posixtimer_rearm().
	 * We re-queue ->sigq and drop ->it_lock().
	 * posixtimer_rearm() locks the timer
	 * and re-schedules it while ->sigq is pending.
	 * Not really bad, but not that we want.
	 */
	timr->sigq->info.si_sys_private = si_private;

	rcu_read_lock();
	task = pid_task(timr->it_pid, PIDTYPE_PID);
	if (task) {
		shared = !(timr->it_sigev_notify & SIGEV_THREAD_ID);
		ret = send_sigqueue(timr->sigq, task, shared);
	}
	rcu_read_unlock();
	/* If we failed to send the signal the timer stops. */
	return ret > 0;
}