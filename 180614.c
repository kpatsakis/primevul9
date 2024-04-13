static inline void free_devinfo_mem(void *vaddr)
{
	kmem_cache_free(iommu_devinfo_cache, vaddr);
}