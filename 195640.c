static void update_backups(struct super_block *sb,
			   int blk_off, char *data, int size)
{
	struct ext4_sb_info *sbi = EXT4_SB(sb);
	const ext4_group_t last = sbi->s_groups_count;
	const int bpg = EXT4_BLOCKS_PER_GROUP(sb);
	unsigned three = 1;
	unsigned five = 5;
	unsigned seven = 7;
	ext4_group_t group;
	int rest = sb->s_blocksize - size;
	handle_t *handle;
	int err = 0, err2;

	handle = ext4_journal_start_sb(sb, EXT4_MAX_TRANS_DATA);
	if (IS_ERR(handle)) {
		group = 1;
		err = PTR_ERR(handle);
		goto exit_err;
	}

	ext4_superblock_csum_set(sb, (struct ext4_super_block *)data);

	while ((group = ext4_list_backups(sb, &three, &five, &seven)) < last) {
		struct buffer_head *bh;

		/* Out of journal space, and can't get more - abort - so sad */
		if (ext4_handle_valid(handle) &&
		    handle->h_buffer_credits == 0 &&
		    ext4_journal_extend(handle, EXT4_MAX_TRANS_DATA) &&
		    (err = ext4_journal_restart(handle, EXT4_MAX_TRANS_DATA)))
			break;

		bh = sb_getblk(sb, group * bpg + blk_off);
		if (!bh) {
			err = -EIO;
			break;
		}
		ext4_debug("update metadata backup %#04lx\n",
			  (unsigned long)bh->b_blocknr);
		if ((err = ext4_journal_get_write_access(handle, bh)))
			break;
		lock_buffer(bh);
		memcpy(bh->b_data, data, size);
		if (rest)
			memset(bh->b_data + size, 0, rest);
		set_buffer_uptodate(bh);
		unlock_buffer(bh);
		err = ext4_handle_dirty_metadata(handle, NULL, bh);
		if (unlikely(err))
			ext4_std_error(sb, err);
		brelse(bh);
	}
	if ((err2 = ext4_journal_stop(handle)) && !err)
		err = err2;

	/*
	 * Ugh! Need to have e2fsck write the backup copies.  It is too
	 * late to revert the resize, we shouldn't fail just because of
	 * the backup copies (they are only needed in case of corruption).
	 *
	 * However, if we got here we have a journal problem too, so we
	 * can't really start a transaction to mark the superblock.
	 * Chicken out and just set the flag on the hope it will be written
	 * to disk, and if not - we will simply wait until next fsck.
	 */
exit_err:
	if (err) {
		ext4_warning(sb, "can't update backup for group %u (err %d), "
			     "forcing fsck on next reboot", group, err);
		sbi->s_mount_state &= ~EXT4_VALID_FS;
		sbi->s_es->s_state &= cpu_to_le16(~EXT4_VALID_FS);
		mark_buffer_dirty(sbi->s_sbh);
	}
}