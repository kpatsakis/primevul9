dmar_search_domain_by_dev_info(int segment, int bus, int devfn)
{
	struct device_domain_info *info;

	list_for_each_entry(info, &device_domain_list, global)
		if (info->iommu->segment == segment && info->bus == bus &&
		    info->devfn == devfn)
			return info;

	return NULL;
}