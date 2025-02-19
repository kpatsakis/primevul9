int __ext4_xattr_set_credits(struct super_block *sb, struct inode *inode,
			     struct buffer_head *block_bh, size_t value_len,
			     bool is_create)
{
	int credits;
	int blocks;

	/*
	 * 1) Owner inode update
	 * 2) Ref count update on old xattr block
	 * 3) new xattr block
	 * 4) block bitmap update for new xattr block
	 * 5) group descriptor for new xattr block
	 * 6) block bitmap update for old xattr block
	 * 7) group descriptor for old block
	 *
	 * 6 & 7 can happen if we have two racing threads T_a and T_b
	 * which are each trying to set an xattr on inodes I_a and I_b
	 * which were both initially sharing an xattr block.
	 */
	credits = 7;

	/* Quota updates. */
	credits += EXT4_MAXQUOTAS_TRANS_BLOCKS(sb);

	/*
	 * In case of inline data, we may push out the data to a block,
	 * so we need to reserve credits for this eventuality
	 */
	if (inode && ext4_has_inline_data(inode))
		credits += ext4_writepage_trans_blocks(inode) + 1;

	/* We are done if ea_inode feature is not enabled. */
	if (!ext4_has_feature_ea_inode(sb))
		return credits;

	/* New ea_inode, inode map, block bitmap, group descriptor. */
	credits += 4;

	/* Data blocks. */
	blocks = (value_len + sb->s_blocksize - 1) >> sb->s_blocksize_bits;

	/* Indirection block or one level of extent tree. */
	blocks += 1;

	/* Block bitmap and group descriptor updates for each block. */
	credits += blocks * 2;

	/* Blocks themselves. */
	credits += blocks;

	if (!is_create) {
		/* Dereference ea_inode holding old xattr value.
		 * Old ea_inode, inode map, block bitmap, group descriptor.
		 */
		credits += 4;

		/* Data blocks for old ea_inode. */
		blocks = XATTR_SIZE_MAX >> sb->s_blocksize_bits;

		/* Indirection block or one level of extent tree for old
		 * ea_inode.
		 */
		blocks += 1;

		/* Block bitmap and group descriptor updates for each block. */
		credits += blocks * 2;
	}

	/* We may need to clone the existing xattr block in which case we need
	 * to increment ref counts for existing ea_inodes referenced by it.
	 */
	if (block_bh) {
		struct ext4_xattr_entry *entry = BFIRST(block_bh);

		for (; !IS_LAST_ENTRY(entry); entry = EXT4_XATTR_NEXT(entry))
			if (entry->e_value_inum)
				/* Ref count update on ea_inode. */
				credits += 1;
	}
	return credits;
}