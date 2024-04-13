static inline void *alloc_domain_mem(void)
{
	return kmem_cache_alloc(iommu_domain_cache, GFP_ATOMIC);
}