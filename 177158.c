int ext4_expand_extra_isize_ea(struct inode *inode, int new_extra_isize,
			       struct ext4_inode *raw_inode, handle_t *handle)
{
	struct ext4_xattr_ibody_header *header;
	struct buffer_head *bh;
	struct ext4_sb_info *sbi = EXT4_SB(inode->i_sb);
	static unsigned int mnt_count;
	size_t min_offs;
	size_t ifree, bfree;
	int total_ino;
	void *base, *end;
	int error = 0, tried_min_extra_isize = 0;
	int s_min_extra_isize = le16_to_cpu(sbi->s_es->s_min_extra_isize);
	int isize_diff;	/* How much do we need to grow i_extra_isize */

retry:
	isize_diff = new_extra_isize - EXT4_I(inode)->i_extra_isize;
	if (EXT4_I(inode)->i_extra_isize >= new_extra_isize)
		return 0;

	header = IHDR(inode, raw_inode);

	/*
	 * Check if enough free space is available in the inode to shift the
	 * entries ahead by new_extra_isize.
	 */

	base = IFIRST(header);
	end = (void *)raw_inode + EXT4_SB(inode->i_sb)->s_inode_size;
	min_offs = end - base;
	total_ino = sizeof(struct ext4_xattr_ibody_header);

	error = xattr_check_inode(inode, header, end);
	if (error)
		goto cleanup;

	ifree = ext4_xattr_free_space(base, &min_offs, base, &total_ino);
	if (ifree >= isize_diff)
		goto shift;

	/*
	 * Enough free space isn't available in the inode, check if
	 * EA block can hold new_extra_isize bytes.
	 */
	if (EXT4_I(inode)->i_file_acl) {
		bh = sb_bread(inode->i_sb, EXT4_I(inode)->i_file_acl);
		error = -EIO;
		if (!bh)
			goto cleanup;
		error = ext4_xattr_check_block(inode, bh);
		if (error)
			goto cleanup;
		base = BHDR(bh);
		end = bh->b_data + bh->b_size;
		min_offs = end - base;
		bfree = ext4_xattr_free_space(BFIRST(bh), &min_offs, base,
					      NULL);
		brelse(bh);
		if (bfree + ifree < isize_diff) {
			if (!tried_min_extra_isize && s_min_extra_isize) {
				tried_min_extra_isize++;
				new_extra_isize = s_min_extra_isize;
				goto retry;
			}
			error = -ENOSPC;
			goto cleanup;
		}
	} else {
		bfree = inode->i_sb->s_blocksize;
	}

	error = ext4_xattr_make_inode_space(handle, inode, raw_inode,
					    isize_diff, ifree, bfree,
					    &total_ino);
	if (error) {
		if (error == -ENOSPC && !tried_min_extra_isize &&
		    s_min_extra_isize) {
			tried_min_extra_isize++;
			new_extra_isize = s_min_extra_isize;
			goto retry;
		}
		goto cleanup;
	}
shift:
	/* Adjust the offsets and shift the remaining entries ahead */
	ext4_xattr_shift_entries(IFIRST(header), EXT4_I(inode)->i_extra_isize
			- new_extra_isize, (void *)raw_inode +
			EXT4_GOOD_OLD_INODE_SIZE + new_extra_isize,
			(void *)header, total_ino);
	EXT4_I(inode)->i_extra_isize = new_extra_isize;

cleanup:
	if (error && (mnt_count != le16_to_cpu(sbi->s_es->s_mnt_count))) {
		ext4_warning(inode->i_sb, "Unable to expand inode %lu. Delete some EAs or run e2fsck.",
			     inode->i_ino);
		mnt_count = le16_to_cpu(sbi->s_es->s_mnt_count);
	}
	return error;
}