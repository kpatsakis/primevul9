static inline unsigned long aligned_nrpages(unsigned long host_addr,
					    size_t size)
{
	host_addr &= ~PAGE_MASK;
	return PAGE_ALIGN(host_addr + size) >> VTD_PAGE_SHIFT;
}