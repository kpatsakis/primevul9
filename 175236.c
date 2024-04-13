void *netdev_alloc_frag(unsigned int fragsz)
{
	return __netdev_alloc_frag(fragsz, GFP_ATOMIC | __GFP_COLD);
}