static void mce_timer_fn(struct timer_list *t)
{
	struct timer_list *cpu_t = this_cpu_ptr(&mce_timer);
	unsigned long iv;

	WARN_ON(cpu_t != t);

	iv = __this_cpu_read(mce_next_interval);

	if (mce_available(this_cpu_ptr(&cpu_info))) {
		machine_check_poll(0, this_cpu_ptr(&mce_poll_banks));

		if (mce_intel_cmci_poll()) {
			iv = mce_adjust_timer(iv);
			goto done;
		}
	}

	/*
	 * Alert userspace if needed. If we logged an MCE, reduce the polling
	 * interval, otherwise increase the polling interval.
	 */
	if (mce_notify_irq())
		iv = max(iv / 2, (unsigned long) HZ/100);
	else
		iv = min(iv * 2, round_jiffies_relative(check_interval * HZ));

done:
	__this_cpu_write(mce_next_interval, iv);
	__start_timer(t, iv);
}