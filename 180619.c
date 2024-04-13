static void free_domain_mem(void *vaddr)
{
	kmem_cache_free(iommu_domain_cache, vaddr);
}