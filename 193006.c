static struct k_itimer *__lock_timer(timer_t timer_id, unsigned long *flags)
{
	struct k_itimer *timr;

	/*
	 * timer_t could be any type >= int and we want to make sure any
	 * @timer_id outside positive int range fails lookup.
	 */
	if ((unsigned long long)timer_id > INT_MAX)
		return NULL;

	rcu_read_lock();
	timr = posix_timer_by_id(timer_id);
	if (timr) {
		spin_lock_irqsave(&timr->it_lock, *flags);
		if (timr->it_signal == current->signal) {
			rcu_read_unlock();
			return timr;
		}
		spin_unlock_irqrestore(&timr->it_lock, *flags);
	}
	rcu_read_unlock();

	return NULL;
}