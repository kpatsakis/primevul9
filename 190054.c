static void mmu_free_pte_list_desc(struct pte_list_desc *pte_list_desc)
{
	kmem_cache_free(pte_list_desc_cache, pte_list_desc);
}