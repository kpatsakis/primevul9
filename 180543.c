static int intel_iommu_cpu_dead(unsigned int cpu)
{
	free_all_cpu_cached_iovas(cpu);
	return 0;
}