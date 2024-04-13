static inline void *unmask_page(unsigned long p)
{
	return (void *)(uintptr_t)(p & PAGE_MASK);
}