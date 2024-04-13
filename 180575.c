static void __init iommu_exit_mempool(void)
{
	kmem_cache_destroy(iommu_devinfo_cache);
	kmem_cache_destroy(iommu_domain_cache);
	iova_cache_put();
}