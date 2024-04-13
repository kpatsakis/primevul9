static void pmd_frag_destroy(void *pmd_frag)
{
	int count;
	struct page *page;

	page = virt_to_page(pmd_frag);
	/* drop all the pending references */
	count = ((unsigned long)pmd_frag & ~PAGE_MASK) >> PMD_FRAG_SIZE_SHIFT;
	/* We allow PTE_FRAG_NR fragments from a PTE page */
	if (atomic_sub_and_test(PMD_FRAG_NR - count, &page->pt_frag_refcount)) {
		pgtable_pmd_page_dtor(page);
		__free_page(page);
	}
}