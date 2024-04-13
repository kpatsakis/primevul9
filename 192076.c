static void mce_disable_cpu(void)
{
	if (!mce_available(raw_cpu_ptr(&cpu_info)))
		return;

	if (!cpuhp_tasks_frozen)
		cmci_clear();

	vendor_disable_error_reporting();
}