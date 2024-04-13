vgetcpu_cpu_notifier(struct notifier_block *n, unsigned long action, void *arg)
{
	long cpu = (long)arg;

	if (action == CPU_ONLINE || action == CPU_ONLINE_FROZEN)
		smp_call_function_single(cpu, vgetcpu_cpu_init, NULL, 1);

	return NOTIFY_DONE;
}