int for_each_device_domain(int (*fn)(struct device_domain_info *info,
				     void *data), void *data)
{
	int ret = 0;
	struct device_domain_info *info;

	assert_spin_locked(&device_domain_lock);
	list_for_each_entry(info, &device_domain_list, global) {
		ret = fn(info, data);
		if (ret)
			return ret;
	}

	return 0;
}