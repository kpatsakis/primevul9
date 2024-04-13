static void k_itimer_rcu_free(struct rcu_head *head)
{
	struct k_itimer *tmr = container_of(head, struct k_itimer, it.rcu);

	kmem_cache_free(posix_timers_cache, tmr);
}