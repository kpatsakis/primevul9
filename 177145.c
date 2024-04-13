ext4_xattr_ibody_list(struct dentry *dentry, char *buffer, size_t buffer_size)
{
	struct inode *inode = d_inode(dentry);
	struct ext4_xattr_ibody_header *header;
	struct ext4_inode *raw_inode;
	struct ext4_iloc iloc;
	void *end;
	int error;

	if (!ext4_test_inode_state(inode, EXT4_STATE_XATTR))
		return 0;
	error = ext4_get_inode_loc(inode, &iloc);
	if (error)
		return error;
	raw_inode = ext4_raw_inode(&iloc);
	header = IHDR(inode, raw_inode);
	end = (void *)raw_inode + EXT4_SB(inode->i_sb)->s_inode_size;
	error = xattr_check_inode(inode, header, end);
	if (error)
		goto cleanup;
	error = ext4_xattr_list_entries(dentry, IFIRST(header),
					buffer, buffer_size);

cleanup:
	brelse(iloc.bh);
	return error;
}