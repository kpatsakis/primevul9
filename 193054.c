static int posix_timer_add(struct k_itimer *timer)
{
	struct signal_struct *sig = current->signal;
	int first_free_id = sig->posix_timer_id;
	struct hlist_head *head;
	int ret = -ENOENT;

	do {
		spin_lock(&hash_lock);
		head = &posix_timers_hashtable[hash(sig, sig->posix_timer_id)];
		if (!__posix_timers_find(head, sig, sig->posix_timer_id)) {
			hlist_add_head_rcu(&timer->t_hash, head);
			ret = sig->posix_timer_id;
		}
		if (++sig->posix_timer_id < 0)
			sig->posix_timer_id = 0;
		if ((sig->posix_timer_id == first_free_id) && (ret == -ENOENT))
			/* Loop over all possible ids completed */
			ret = -EAGAIN;
		spin_unlock(&hash_lock);
	} while (ret == -ENOENT);
	return ret;
}