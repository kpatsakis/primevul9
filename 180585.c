static void iommu_flush_dev_iotlb(struct dmar_domain *domain,
				  u64 addr, unsigned mask)
{
	u16 sid, qdep;
	unsigned long flags;
	struct device_domain_info *info;

	if (!domain->has_iotlb_device)
		return;

	spin_lock_irqsave(&device_domain_lock, flags);
	list_for_each_entry(info, &domain->devices, link) {
		if (!info->ats_enabled)
			continue;

		sid = info->bus << 8 | info->devfn;
		qdep = info->ats_qdep;
		qi_flush_dev_iotlb(info->iommu, sid, info->pfsid,
				qdep, addr, mask);
	}
	spin_unlock_irqrestore(&device_domain_lock, flags);
}