static void ds_clear_cea(void *cea, size_t size)
{
	unsigned long start = (unsigned long)cea;
	size_t msz = 0;

	preempt_disable();
	for (; msz < size; msz += PAGE_SIZE, cea += PAGE_SIZE)
		cea_set_pte(cea, 0, PAGE_NONE);

	flush_tlb_kernel_range(start, start + size);
	preempt_enable();
}