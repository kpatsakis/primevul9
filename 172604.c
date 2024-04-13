static int scrub_checksum(struct scrub_block *sblock)
{
	u64 flags;
	int ret;

	/*
	 * No need to initialize these stats currently,
	 * because this function only use return value
	 * instead of these stats value.
	 *
	 * Todo:
	 * always use stats
	 */
	sblock->header_error = 0;
	sblock->generation_error = 0;
	sblock->checksum_error = 0;

	WARN_ON(sblock->page_count < 1);
	flags = sblock->pagev[0]->flags;
	ret = 0;
	if (flags & BTRFS_EXTENT_FLAG_DATA)
		ret = scrub_checksum_data(sblock);
	else if (flags & BTRFS_EXTENT_FLAG_TREE_BLOCK)
		ret = scrub_checksum_tree_block(sblock);
	else if (flags & BTRFS_EXTENT_FLAG_SUPER)
		(void)scrub_checksum_super(sblock);
	else
		WARN_ON(1);
	if (ret)
		scrub_handle_errored_block(sblock);

	return ret;
}