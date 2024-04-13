static void intel_iommu_init_qi(struct intel_iommu *iommu)
{
	/*
	 * Start from the sane iommu hardware state.
	 * If the queued invalidation is already initialized by us
	 * (for example, while enabling interrupt-remapping) then
	 * we got the things already rolling from a sane state.
	 */
	if (!iommu->qi) {
		/*
		 * Clear any previous faults.
		 */
		dmar_fault(-1, iommu);
		/*
		 * Disable queued invalidation if supported and already enabled
		 * before OS handover.
		 */
		dmar_disable_qi(iommu);
	}

	if (dmar_enable_qi(iommu)) {
		/*
		 * Queued Invalidate not enabled, use Register Based Invalidate
		 */
		iommu->flush.flush_context = __iommu_flush_context;
		iommu->flush.flush_iotlb = __iommu_flush_iotlb;
		pr_info("%s: Using Register based invalidation\n",
			iommu->name);
	} else {
		iommu->flush.flush_context = qi_flush_context;
		iommu->flush.flush_iotlb = qi_flush_iotlb;
		pr_info("%s: Using Queued invalidation\n", iommu->name);
	}
}