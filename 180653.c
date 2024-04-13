static void free_all_cpu_cached_iovas(unsigned int cpu)
{
	int i;

	for (i = 0; i < g_num_of_iommus; i++) {
		struct intel_iommu *iommu = g_iommus[i];
		struct dmar_domain *domain;
		int did;

		if (!iommu)
			continue;

		for (did = 0; did < cap_ndoms(iommu->cap); did++) {
			domain = get_iommu_domain(iommu, (u16)did);

			if (!domain)
				continue;
			free_cpu_cached_iovas(cpu, &domain->iovad);
		}
	}
}