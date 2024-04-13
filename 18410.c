int timers_dead_cpu(unsigned int cpu)
{
	struct timer_base *old_base;
	struct timer_base *new_base;
	int b, i;

	BUG_ON(cpu_online(cpu));

	for (b = 0; b < NR_BASES; b++) {
		old_base = per_cpu_ptr(&timer_bases[b], cpu);
		new_base = get_cpu_ptr(&timer_bases[b]);
		/*
		 * The caller is globally serialized and nobody else
		 * takes two locks at once, deadlock is not possible.
		 */
		raw_spin_lock_irq(&new_base->lock);
		raw_spin_lock_nested(&old_base->lock, SINGLE_DEPTH_NESTING);

		/*
		 * The current CPUs base clock might be stale. Update it
		 * before moving the timers over.
		 */
		forward_timer_base(new_base);

		BUG_ON(old_base->running_timer);

		for (i = 0; i < WHEEL_SIZE; i++)
			migrate_timer_list(new_base, old_base->vectors + i);

		raw_spin_unlock(&old_base->lock);
		raw_spin_unlock_irq(&new_base->lock);
		put_cpu_ptr(&timer_bases);
	}
	return 0;
}