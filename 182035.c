__visible void kvm_lock_spinning(struct arch_spinlock *lock, __ticket_t want)
{
	struct kvm_lock_waiting *w;
	int cpu;
	u64 start;
	unsigned long flags;

	if (in_nmi())
		return;

	w = this_cpu_ptr(&klock_waiting);
	cpu = smp_processor_id();
	start = spin_time_start();

	/*
	 * Make sure an interrupt handler can't upset things in a
	 * partially setup state.
	 */
	local_irq_save(flags);

	/*
	 * The ordering protocol on this is that the "lock" pointer
	 * may only be set non-NULL if the "want" ticket is correct.
	 * If we're updating "want", we must first clear "lock".
	 */
	w->lock = NULL;
	smp_wmb();
	w->want = want;
	smp_wmb();
	w->lock = lock;

	add_stats(TAKEN_SLOW, 1);

	/*
	 * This uses set_bit, which is atomic but we should not rely on its
	 * reordering gurantees. So barrier is needed after this call.
	 */
	cpumask_set_cpu(cpu, &waiting_cpus);

	barrier();

	/*
	 * Mark entry to slowpath before doing the pickup test to make
	 * sure we don't deadlock with an unlocker.
	 */
	__ticket_enter_slowpath(lock);

	/*
	 * check again make sure it didn't become free while
	 * we weren't looking.
	 */
	if (ACCESS_ONCE(lock->tickets.head) == want) {
		add_stats(TAKEN_SLOW_PICKUP, 1);
		goto out;
	}

	/*
	 * halt until it's our turn and kicked. Note that we do safe halt
	 * for irq enabled case to avoid hang when lock info is overwritten
	 * in irq spinlock slowpath and no spurious interrupt occur to save us.
	 */
	if (arch_irqs_disabled_flags(flags))
		halt();
	else
		safe_halt();

out:
	cpumask_clear_cpu(cpu, &waiting_cpus);
	w->lock = NULL;
	local_irq_restore(flags);
	spin_time_accum_blocked(start);
}