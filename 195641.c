static ssize_t ext4_quota_write(struct super_block *sb, int type,
				const char *data, size_t len, loff_t off)
{
	struct inode *inode = sb_dqopt(sb)->files[type];
	ext4_lblk_t blk = off >> EXT4_BLOCK_SIZE_BITS(sb);
	int err = 0;
	int offset = off & (sb->s_blocksize - 1);
	struct buffer_head *bh;
	handle_t *handle = journal_current_handle();

	if (EXT4_SB(sb)->s_journal && !handle) {
		ext4_msg(sb, KERN_WARNING, "Quota write (off=%llu, len=%llu)"
			" cancelled because transaction is not started",
			(unsigned long long)off, (unsigned long long)len);
		return -EIO;
	}
	/*
	 * Since we account only one data block in transaction credits,
	 * then it is impossible to cross a block boundary.
	 */
	if (sb->s_blocksize - offset < len) {
		ext4_msg(sb, KERN_WARNING, "Quota write (off=%llu, len=%llu)"
			" cancelled because not block aligned",
			(unsigned long long)off, (unsigned long long)len);
		return -EIO;
	}

	bh = ext4_bread(handle, inode, blk, 1, &err);
	if (!bh)
		goto out;
	err = ext4_journal_get_write_access(handle, bh);
	if (err) {
		brelse(bh);
		goto out;
	}
	lock_buffer(bh);
	memcpy(bh->b_data+offset, data, len);
	flush_dcache_page(bh->b_page);
	unlock_buffer(bh);
	err = ext4_handle_dirty_metadata(handle, NULL, bh);
	brelse(bh);
out:
	if (err)
		return err;
	if (inode->i_size < off + len) {
		i_size_write(inode, off + len);
		EXT4_I(inode)->i_disksize = inode->i_size;
		ext4_mark_inode_dirty(handle, inode);
	}
	return len;
}