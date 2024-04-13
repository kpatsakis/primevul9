static void disable_dmar_iommu(struct intel_iommu *iommu)
{
	struct device_domain_info *info, *tmp;
	unsigned long flags;

	if (!iommu->domains || !iommu->domain_ids)
		return;

again:
	spin_lock_irqsave(&device_domain_lock, flags);
	list_for_each_entry_safe(info, tmp, &device_domain_list, global) {
		struct dmar_domain *domain;

		if (info->iommu != iommu)
			continue;

		if (!info->dev || !info->domain)
			continue;

		domain = info->domain;

		__dmar_remove_one_dev_info(info);

		if (!domain_type_is_vm_or_si(domain)) {
			/*
			 * The domain_exit() function  can't be called under
			 * device_domain_lock, as it takes this lock itself.
			 * So release the lock here and re-run the loop
			 * afterwards.
			 */
			spin_unlock_irqrestore(&device_domain_lock, flags);
			domain_exit(domain);
			goto again;
		}
	}
	spin_unlock_irqrestore(&device_domain_lock, flags);

	if (iommu->gcmd & DMA_GCMD_TE)
		iommu_disable_translation(iommu);
}