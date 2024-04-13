static int find_live_mirror(struct btrfs_fs_info *fs_info,
			    struct map_lookup *map, int first,
			    int dev_replace_is_ongoing)
{
	int i;
	int num_stripes;
	int preferred_mirror;
	int tolerance;
	struct btrfs_device *srcdev;

	ASSERT((map->type &
		 (BTRFS_BLOCK_GROUP_RAID1 | BTRFS_BLOCK_GROUP_RAID10)));

	if (map->type & BTRFS_BLOCK_GROUP_RAID10)
		num_stripes = map->sub_stripes;
	else
		num_stripes = map->num_stripes;

	preferred_mirror = first + current->pid % num_stripes;

	if (dev_replace_is_ongoing &&
	    fs_info->dev_replace.cont_reading_from_srcdev_mode ==
	     BTRFS_DEV_REPLACE_ITEM_CONT_READING_FROM_SRCDEV_MODE_AVOID)
		srcdev = fs_info->dev_replace.srcdev;
	else
		srcdev = NULL;

	/*
	 * try to avoid the drive that is the source drive for a
	 * dev-replace procedure, only choose it if no other non-missing
	 * mirror is available
	 */
	for (tolerance = 0; tolerance < 2; tolerance++) {
		if (map->stripes[preferred_mirror].dev->bdev &&
		    (tolerance || map->stripes[preferred_mirror].dev != srcdev))
			return preferred_mirror;
		for (i = first; i < first + num_stripes; i++) {
			if (map->stripes[i].dev->bdev &&
			    (tolerance || map->stripes[i].dev != srcdev))
				return i;
		}
	}

	/* we couldn't find one that doesn't fail.  Just return something
	 * and the io error handling code will clean up eventually
	 */
	return preferred_mirror;
}