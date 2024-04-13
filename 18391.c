static void timer_sync_wait_running(struct timer_base *base)
{
	if (atomic_read(&base->timer_waiters)) {
		spin_unlock(&base->expiry_lock);
		spin_lock(&base->expiry_lock);
	}
}