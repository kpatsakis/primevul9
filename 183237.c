int for_each_device_domain(int (*fn)(struct device_domain_info *info,
				     void *data), void *data)
{
	int ret = 0;
	unsigned long flags;
	struct device_domain_info *info;

	spin_lock_irqsave(&device_domain_lock, flags);
	list_for_each_entry(info, &device_domain_list, global) {
		ret = fn(info, data);
		if (ret) {
			spin_unlock_irqrestore(&device_domain_lock, flags);
			return ret;
		}
	}
	spin_unlock_irqrestore(&device_domain_lock, flags);

	return 0;
}