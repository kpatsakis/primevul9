static struct buffer_head *ext4_xattr_get_block(struct inode *inode)
{
	struct buffer_head *bh;
	int error;

	if (!EXT4_I(inode)->i_file_acl)
		return NULL;
	bh = sb_bread(inode->i_sb, EXT4_I(inode)->i_file_acl);
	if (!bh)
		return ERR_PTR(-EIO);
	error = ext4_xattr_check_block(inode, bh);
	if (error)
		return ERR_PTR(error);
	return bh;
}