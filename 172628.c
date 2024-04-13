static int btrfs_read_disk_super(struct block_device *bdev, u64 bytenr,
				 struct page **page,
				 struct btrfs_super_block **disk_super)
{
	void *p;
	pgoff_t index;

	/* make sure our super fits in the device */
	if (bytenr + PAGE_SIZE >= i_size_read(bdev->bd_inode))
		return 1;

	/* make sure our super fits in the page */
	if (sizeof(**disk_super) > PAGE_SIZE)
		return 1;

	/* make sure our super doesn't straddle pages on disk */
	index = bytenr >> PAGE_SHIFT;
	if ((bytenr + sizeof(**disk_super) - 1) >> PAGE_SHIFT != index)
		return 1;

	/* pull in the page with our super */
	*page = read_cache_page_gfp(bdev->bd_inode->i_mapping,
				   index, GFP_KERNEL);

	if (IS_ERR_OR_NULL(*page))
		return 1;

	p = kmap(*page);

	/* align our pointer to the offset of the super block */
	*disk_super = p + offset_in_page(bytenr);

	if (btrfs_super_bytenr(*disk_super) != bytenr ||
	    btrfs_super_magic(*disk_super) != BTRFS_MAGIC) {
		btrfs_release_disk_super(*page);
		return 1;
	}

	if ((*disk_super)->label[0] &&
		(*disk_super)->label[BTRFS_LABEL_SIZE - 1])
		(*disk_super)->label[BTRFS_LABEL_SIZE - 1] = '\0';

	return 0;
}