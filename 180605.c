static void dmar_remove_one_dev_info(struct dmar_domain *domain,
				     struct device *dev)
{
	struct device_domain_info *info;
	unsigned long flags;

	spin_lock_irqsave(&device_domain_lock, flags);
	info = dev->archdata.iommu;
	__dmar_remove_one_dev_info(info);
	spin_unlock_irqrestore(&device_domain_lock, flags);
}