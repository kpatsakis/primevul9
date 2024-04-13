static int __iommu_calculate_agaw(struct intel_iommu *iommu, int max_gaw)
{
	unsigned long sagaw;
	int agaw = -1;

	sagaw = cap_sagaw(iommu->cap);
	for (agaw = width_to_agaw(max_gaw);
	     agaw >= 0; agaw--) {
		if (test_bit(agaw, &sagaw))
			break;
	}

	return agaw;
}