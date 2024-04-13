static int __init si_domain_init(int hw)
{
	int nid, ret = 0;

	si_domain = alloc_domain(DOMAIN_FLAG_STATIC_IDENTITY);
	if (!si_domain)
		return -EFAULT;

	if (md_domain_init(si_domain, DEFAULT_DOMAIN_ADDRESS_WIDTH)) {
		domain_exit(si_domain);
		return -EFAULT;
	}

	pr_debug("Identity mapping domain allocated\n");

	if (hw)
		return 0;

	for_each_online_node(nid) {
		unsigned long start_pfn, end_pfn;
		int i;

		for_each_mem_pfn_range(i, nid, &start_pfn, &end_pfn, NULL) {
			ret = iommu_domain_identity_map(si_domain,
					PFN_PHYS(start_pfn), PFN_PHYS(end_pfn));
			if (ret)
				return ret;
		}
	}

	return 0;
}