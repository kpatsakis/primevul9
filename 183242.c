static int intel_iommu_add(struct dmar_drhd_unit *dmaru)
{
	int sp, ret;
	struct intel_iommu *iommu = dmaru->iommu;

	if (g_iommus[iommu->seq_id])
		return 0;

	if (hw_pass_through && !ecap_pass_through(iommu->ecap)) {
		pr_warn("%s: Doesn't support hardware pass through.\n",
			iommu->name);
		return -ENXIO;
	}
	if (!ecap_sc_support(iommu->ecap) &&
	    domain_update_iommu_snooping(iommu)) {
		pr_warn("%s: Doesn't support snooping.\n",
			iommu->name);
		return -ENXIO;
	}
	sp = domain_update_iommu_superpage(iommu) - 1;
	if (sp >= 0 && !(cap_super_page_val(iommu->cap) & (1 << sp))) {
		pr_warn("%s: Doesn't support large page.\n",
			iommu->name);
		return -ENXIO;
	}

	/*
	 * Disable translation if already enabled prior to OS handover.
	 */
	if (iommu->gcmd & DMA_GCMD_TE)
		iommu_disable_translation(iommu);

	g_iommus[iommu->seq_id] = iommu;
	ret = iommu_init_domains(iommu);
	if (ret == 0)
		ret = iommu_alloc_root_entry(iommu);
	if (ret)
		goto out;

#ifdef CONFIG_INTEL_IOMMU_SVM
	if (pasid_supported(iommu))
		intel_svm_init(iommu);
#endif

	if (dmaru->ignored) {
		/*
		 * we always have to disable PMRs or DMA may fail on this device
		 */
		if (force_on)
			iommu_disable_protect_mem_regions(iommu);
		return 0;
	}

	intel_iommu_init_qi(iommu);
	iommu_flush_write_buffer(iommu);

#ifdef CONFIG_INTEL_IOMMU_SVM
	if (pasid_supported(iommu) && ecap_prs(iommu->ecap)) {
		ret = intel_svm_enable_prq(iommu);
		if (ret)
			goto disable_iommu;
	}
#endif
	ret = dmar_set_interrupt(iommu);
	if (ret)
		goto disable_iommu;

	iommu_set_root_entry(iommu);
	iommu->flush.flush_context(iommu, 0, 0, 0, DMA_CCMD_GLOBAL_INVL);
	iommu->flush.flush_iotlb(iommu, 0, 0, 0, DMA_TLB_GLOBAL_FLUSH);
	iommu_enable_translation(iommu);

	iommu_disable_protect_mem_regions(iommu);
	return 0;

disable_iommu:
	disable_dmar_iommu(iommu);
out:
	free_dmar_iommu(iommu);
	return ret;
}