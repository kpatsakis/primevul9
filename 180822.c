unsigned long nr_free_pagecache_pages(void)
{
	return nr_free_zone_pages(gfp_zone(GFP_HIGHUSER_MOVABLE));
}