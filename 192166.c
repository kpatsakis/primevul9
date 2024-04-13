static void __start_timer(struct timer_list *t, unsigned long interval)
{
	unsigned long when = jiffies + interval;
	unsigned long flags;

	local_irq_save(flags);

	if (!timer_pending(t) || time_before(when, t->expires))
		mod_timer(t, round_jiffies(when));

	local_irq_restore(flags);
}