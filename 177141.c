int ext4_xattr_set_credits(struct inode *inode, size_t value_len,
			   bool is_create, int *credits)
{
	struct buffer_head *bh;
	int err;

	*credits = 0;

	if (!EXT4_SB(inode->i_sb)->s_journal)
		return 0;

	down_read(&EXT4_I(inode)->xattr_sem);

	bh = ext4_xattr_get_block(inode);
	if (IS_ERR(bh)) {
		err = PTR_ERR(bh);
	} else {
		*credits = __ext4_xattr_set_credits(inode->i_sb, inode, bh,
						    value_len, is_create);
		brelse(bh);
		err = 0;
	}

	up_read(&EXT4_I(inode)->xattr_sem);
	return err;
}