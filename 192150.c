static void mce_disable_cmci(void *data)
{
	if (!mce_available(raw_cpu_ptr(&cpu_info)))
		return;
	cmci_clear();
}