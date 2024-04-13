void free_pgtable_page(void *vaddr)
{
	free_page((unsigned long)vaddr);
}