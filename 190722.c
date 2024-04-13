static sector_t f2fs_bmap(struct address_space *mapping, sector_t block)
{
	struct inode *inode = mapping->host;

	if (f2fs_has_inline_data(inode))
		return 0;

	/* make sure allocating whole blocks */
	if (mapping_tagged(mapping, PAGECACHE_TAG_DIRTY))
		filemap_write_and_wait(mapping);

	return generic_block_bmap(mapping, block, get_data_block_bmap);
}