static struct k_itimer *posix_timer_by_id(timer_t id)
{
	struct signal_struct *sig = current->signal;
	struct hlist_head *head = &posix_timers_hashtable[hash(sig, id)];

	return __posix_timers_find(head, sig, id);
}