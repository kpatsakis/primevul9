SYSCALL_DEFINE1(timer_delete, timer_t, timer_id)
{
	struct k_itimer *timer;
	unsigned long flags;

retry_delete:
	timer = lock_timer(timer_id, &flags);
	if (!timer)
		return -EINVAL;

	if (timer_delete_hook(timer) == TIMER_RETRY) {
		unlock_timer(timer, flags);
		goto retry_delete;
	}

	spin_lock(&current->sighand->siglock);
	list_del(&timer->list);
	spin_unlock(&current->sighand->siglock);
	/*
	 * This keeps any tasks waiting on the spin lock from thinking
	 * they got something (see the lock code above).
	 */
	timer->it_signal = NULL;

	unlock_timer(timer, flags);
	release_posix_timer(timer, IT_ID_SET);
	return 0;
}