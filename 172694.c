static inline int scrub_is_page_on_raid56(struct scrub_page *page)
{
	return page->recover &&
	       (page->recover->bbio->map_type & BTRFS_BLOCK_GROUP_RAID56_MASK);
}