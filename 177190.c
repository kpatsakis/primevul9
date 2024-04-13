int ext4_get_inode_usage(struct inode *inode, qsize_t *usage)
{
	struct ext4_iloc iloc = { .bh = NULL };
	struct buffer_head *bh = NULL;
	struct ext4_inode *raw_inode;
	struct ext4_xattr_ibody_header *header;
	struct ext4_xattr_entry *entry;
	qsize_t ea_inode_refs = 0;
	void *end;
	int ret;

	lockdep_assert_held_read(&EXT4_I(inode)->xattr_sem);

	if (ext4_test_inode_state(inode, EXT4_STATE_XATTR)) {
		ret = ext4_get_inode_loc(inode, &iloc);
		if (ret)
			goto out;
		raw_inode = ext4_raw_inode(&iloc);
		header = IHDR(inode, raw_inode);
		end = (void *)raw_inode + EXT4_SB(inode->i_sb)->s_inode_size;
		ret = xattr_check_inode(inode, header, end);
		if (ret)
			goto out;

		for (entry = IFIRST(header); !IS_LAST_ENTRY(entry);
		     entry = EXT4_XATTR_NEXT(entry))
			if (entry->e_value_inum)
				ea_inode_refs++;
	}

	if (EXT4_I(inode)->i_file_acl) {
		bh = sb_bread(inode->i_sb, EXT4_I(inode)->i_file_acl);
		if (!bh) {
			ret = -EIO;
			goto out;
		}

		ret = ext4_xattr_check_block(inode, bh);
		if (ret)
			goto out;

		for (entry = BFIRST(bh); !IS_LAST_ENTRY(entry);
		     entry = EXT4_XATTR_NEXT(entry))
			if (entry->e_value_inum)
				ea_inode_refs++;
	}
	*usage = ea_inode_refs + 1;
	ret = 0;
out:
	brelse(iloc.bh);
	brelse(bh);
	return ret;
}