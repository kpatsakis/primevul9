int f2fs_reserve_block(struct dnode_of_data *dn, pgoff_t index)
{
	bool need_put = dn->inode_page ? false : true;
	int err;

	err = f2fs_get_dnode_of_data(dn, index, ALLOC_NODE);
	if (err)
		return err;

	if (dn->data_blkaddr == NULL_ADDR)
		err = f2fs_reserve_new_block(dn);
	if (err || need_put)
		f2fs_put_dnode(dn);
	return err;
}