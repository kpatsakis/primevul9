int __meminit init_per_zone_wmark_min(void)
{
	unsigned long lowmem_kbytes;
	int new_min_free_kbytes;

	lowmem_kbytes = nr_free_buffer_pages() * (PAGE_SIZE >> 10);
	new_min_free_kbytes = int_sqrt(lowmem_kbytes * 16);

	if (new_min_free_kbytes > user_min_free_kbytes) {
		min_free_kbytes = new_min_free_kbytes;
		if (min_free_kbytes < 128)
			min_free_kbytes = 128;
		if (min_free_kbytes > 65536)
			min_free_kbytes = 65536;
	} else {
		pr_warn("min_free_kbytes is not updated to %d because user defined value %d is preferred\n",
				new_min_free_kbytes, user_min_free_kbytes);
	}
	setup_per_zone_wmarks();
	refresh_zone_stat_thresholds();
	setup_per_zone_lowmem_reserve();

#ifdef CONFIG_NUMA
	setup_min_unmapped_ratio();
	setup_min_slab_ratio();
#endif

	return 0;
}