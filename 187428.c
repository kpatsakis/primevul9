static struct mdesc_handle * __init mdesc_memblock_alloc(unsigned int mdesc_size)
{
	unsigned int handle_size, alloc_size;
	struct mdesc_handle *hp;
	unsigned long paddr;

	handle_size = (sizeof(struct mdesc_handle) -
		       sizeof(struct mdesc_hdr) +
		       mdesc_size);
	alloc_size = PAGE_ALIGN(handle_size);

	paddr = memblock_phys_alloc(alloc_size, PAGE_SIZE);

	hp = NULL;
	if (paddr) {
		hp = __va(paddr);
		mdesc_handle_init(hp, handle_size, hp);
	}
	return hp;
}