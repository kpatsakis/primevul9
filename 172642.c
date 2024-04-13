static void scrub_recheck_block_checksum(struct scrub_block *sblock)
{
	sblock->header_error = 0;
	sblock->checksum_error = 0;
	sblock->generation_error = 0;

	if (sblock->pagev[0]->flags & BTRFS_EXTENT_FLAG_DATA)
		scrub_checksum_data(sblock);
	else
		scrub_checksum_tree_block(sblock);
}