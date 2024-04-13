static void mmu_destroy_caches(void)
{
	kmem_cache_destroy(pte_list_desc_cache);
	kmem_cache_destroy(mmu_page_header_cache);
}