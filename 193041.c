static struct k_itimer *__posix_timers_find(struct hlist_head *head,
					    struct signal_struct *sig,
					    timer_t id)
{
	struct k_itimer *timer;

	hlist_for_each_entry_rcu(timer, head, t_hash) {
		if ((timer->it_signal == sig) && (timer->it_id == id))
			return timer;
	}
	return NULL;
}