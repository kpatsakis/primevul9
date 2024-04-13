static inline int iommu_domain_cache_init(void)
{
	int ret = 0;

	iommu_domain_cache = kmem_cache_create("iommu_domain",
					 sizeof(struct dmar_domain),
					 0,
					 SLAB_HWCACHE_ALIGN,

					 NULL);
	if (!iommu_domain_cache) {
		pr_err("Couldn't create iommu_domain cache\n");
		ret = -ENOMEM;
	}

	return ret;
}