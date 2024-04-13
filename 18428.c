void add_timer_on(struct timer_list *timer, int cpu)
{
	struct timer_base *new_base, *base;
	unsigned long flags;

	BUG_ON(timer_pending(timer) || !timer->function);

	new_base = get_timer_cpu_base(timer->flags, cpu);

	/*
	 * If @timer was on a different CPU, it should be migrated with the
	 * old base locked to prevent other operations proceeding with the
	 * wrong base locked.  See lock_timer_base().
	 */
	base = lock_timer_base(timer, &flags);
	if (base != new_base) {
		timer->flags |= TIMER_MIGRATING;

		raw_spin_unlock(&base->lock);
		base = new_base;
		raw_spin_lock(&base->lock);
		WRITE_ONCE(timer->flags,
			   (timer->flags & ~TIMER_BASEMASK) | cpu);
	}
	forward_timer_base(base);

	debug_timer_activate(timer);
	internal_add_timer(base, timer);
	raw_spin_unlock_irqrestore(&base->lock, flags);
}