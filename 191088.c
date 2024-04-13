static void svm_hardware_teardown(void)
{
	int cpu;

	if (svm_sev_enabled())
		sev_hardware_teardown();

	for_each_possible_cpu(cpu)
		svm_cpu_uninit(cpu);

	__free_pages(pfn_to_page(iopm_base >> PAGE_SHIFT), IOPM_ALLOC_ORDER);
	iopm_base = 0;
}