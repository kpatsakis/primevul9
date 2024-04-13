static struct mdesc_handle *mdesc_kmalloc(unsigned int mdesc_size)
{
	unsigned int handle_size;
	struct mdesc_handle *hp;
	unsigned long addr;
	void *base;

	handle_size = (sizeof(struct mdesc_handle) -
		       sizeof(struct mdesc_hdr) +
		       mdesc_size);
	base = kmalloc(handle_size + 15, GFP_KERNEL | __GFP_RETRY_MAYFAIL);
	if (!base)
		return NULL;

	addr = (unsigned long)base;
	addr = (addr + 15UL) & ~15UL;
	hp = (struct mdesc_handle *) addr;

	mdesc_handle_init(hp, handle_size, base);

	return hp;
}