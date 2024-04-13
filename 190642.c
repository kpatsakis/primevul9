int f2fs_get_block(struct dnode_of_data *dn, pgoff_t index)
{
	struct extent_info ei  = {0,0,0};
	struct inode *inode = dn->inode;

	if (f2fs_lookup_extent_cache(inode, index, &ei)) {
		dn->data_blkaddr = ei.blk + index - ei.fofs;
		return 0;
	}

	return f2fs_reserve_block(dn, index);
}