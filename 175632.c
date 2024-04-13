static unsigned long kvmgt_virt_to_pfn(void *addr)
{
	return PFN_DOWN(__pa(addr));
}