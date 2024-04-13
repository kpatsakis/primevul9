static int mce_cpu_dead(unsigned int cpu)
{
	mce_intel_hcpu_update(cpu);

	/* intentionally ignoring frozen here */
	if (!cpuhp_tasks_frozen)
		cmci_rediscover();
	return 0;
}