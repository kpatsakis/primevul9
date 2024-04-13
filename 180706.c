static inline void * alloc_devinfo_mem(void)
{
	return kmem_cache_alloc(iommu_devinfo_cache, GFP_ATOMIC);
}