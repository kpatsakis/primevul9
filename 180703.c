static int __init iommu_init_mempool(void)
{
	int ret;
	ret = iova_cache_get();
	if (ret)
		return ret;

	ret = iommu_domain_cache_init();
	if (ret)
		goto domain_error;

	ret = iommu_devinfo_cache_init();
	if (!ret)
		return ret;

	kmem_cache_destroy(iommu_domain_cache);
domain_error:
	iova_cache_put();

	return -ENOMEM;
}