static void domain_remove_dev_info(struct dmar_domain *domain)
{
	struct device_domain_info *info, *tmp;
	unsigned long flags;

	spin_lock_irqsave(&device_domain_lock, flags);
	list_for_each_entry_safe(info, tmp, &domain->devices, link)
		__dmar_remove_one_dev_info(info);
	spin_unlock_irqrestore(&device_domain_lock, flags);
}