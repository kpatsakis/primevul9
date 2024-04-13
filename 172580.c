static void scrub_remap_extent(struct btrfs_fs_info *fs_info,
			       u64 extent_logical, u64 extent_len,
			       u64 *extent_physical,
			       struct btrfs_device **extent_dev,
			       int *extent_mirror_num)
{
	u64 mapped_length;
	struct btrfs_bio *bbio = NULL;
	int ret;

	mapped_length = extent_len;
	ret = btrfs_map_block(fs_info, BTRFS_MAP_READ, extent_logical,
			      &mapped_length, &bbio, 0);
	if (ret || !bbio || mapped_length < extent_len ||
	    !bbio->stripes[0].dev->bdev) {
		btrfs_put_bbio(bbio);
		return;
	}

	*extent_physical = bbio->stripes[0].physical;
	*extent_mirror_num = bbio->mirror_num;
	*extent_dev = bbio->stripes[0].dev;
	btrfs_put_bbio(bbio);
}