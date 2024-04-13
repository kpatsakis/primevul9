static inline void unlink_domain_info(struct device_domain_info *info)
{
	assert_spin_locked(&device_domain_lock);
	list_del(&info->link);
	list_del(&info->global);
	if (info->dev)
		info->dev->archdata.iommu = NULL;
}