void vmalloc_sync_all(void)
{
	unsigned long address;

	if (SHARED_KERNEL_PMD)
		return;

	for (address = VMALLOC_START & PMD_MASK;
	     address >= TASK_SIZE_MAX && address < FIXADDR_TOP;
	     address += PMD_SIZE) {
		struct page *page;

		spin_lock(&pgd_lock);
		list_for_each_entry(page, &pgd_list, lru) {
			spinlock_t *pgt_lock;
			pmd_t *ret;

			/* the pgt_lock only for Xen */
			pgt_lock = &pgd_page_get_mm(page)->page_table_lock;

			spin_lock(pgt_lock);
			ret = vmalloc_sync_one(page_address(page), address);
			spin_unlock(pgt_lock);

			if (!ret)
				break;
		}
		spin_unlock(&pgd_lock);
	}
}