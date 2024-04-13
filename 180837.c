void __init free_area_init(unsigned long *zones_size)
{
	free_area_init_node(0, zones_size,
			__pa(PAGE_OFFSET) >> PAGE_SHIFT, NULL);
	zero_resv_unavail();
}