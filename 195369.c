unsigned long oops_begin(void)
{
	int cpu;
	unsigned long flags;

	oops_enter();

	/* racy, but better than risking deadlock. */
	raw_local_irq_save(flags);
	cpu = smp_processor_id();
	if (!arch_spin_trylock(&die_lock)) {
		if (cpu == die_owner)
			/* nested oops. should stop eventually */;
		else
			arch_spin_lock(&die_lock);
	}
	die_nest_count++;
	die_owner = cpu;
	console_verbose();
	bust_spinlocks(1);
	return flags;
}