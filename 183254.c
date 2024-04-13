static void free_dmar_iommu(struct intel_iommu *iommu)
{
	if ((iommu->domains) && (iommu->domain_ids)) {
		int elems = ALIGN(cap_ndoms(iommu->cap), 256) >> 8;
		int i;

		for (i = 0; i < elems; i++)
			kfree(iommu->domains[i]);
		kfree(iommu->domains);
		kfree(iommu->domain_ids);
		iommu->domains = NULL;
		iommu->domain_ids = NULL;
	}

	g_iommus[iommu->seq_id] = NULL;

	/* free context mapping */
	free_context_table(iommu);

#ifdef CONFIG_INTEL_IOMMU_SVM
	if (pasid_supported(iommu)) {
		if (ecap_prs(iommu->ecap))
			intel_svm_finish_prq(iommu);
	}
#endif
}