void mdesc_populate_present_mask(cpumask_t *mask)
{
	if (tlb_type != hypervisor)
		return;

	ncpus_probed = 0;
	mdesc_iterate_over_cpus(record_one_cpu, NULL, mask);
}