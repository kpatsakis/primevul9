static int add_new_gdb(handle_t *handle, struct inode *inode,
		       ext4_group_t group)
{
	struct super_block *sb = inode->i_sb;
	struct ext4_super_block *es = EXT4_SB(sb)->s_es;
	unsigned long gdb_num = group / EXT4_DESC_PER_BLOCK(sb);
	ext4_fsblk_t gdblock = EXT4_SB(sb)->s_sbh->b_blocknr + 1 + gdb_num;
	struct buffer_head **o_group_desc, **n_group_desc;
	struct buffer_head *dind;
	struct buffer_head *gdb_bh;
	int gdbackups;
	struct ext4_iloc iloc;
	__le32 *data;
	int err;

	if (test_opt(sb, DEBUG))
		printk(KERN_DEBUG
		       "EXT4-fs: ext4_add_new_gdb: adding group block %lu\n",
		       gdb_num);

	/*
	 * If we are not using the primary superblock/GDT copy don't resize,
         * because the user tools have no way of handling this.  Probably a
         * bad time to do it anyways.
         */
	if (EXT4_SB(sb)->s_sbh->b_blocknr !=
	    le32_to_cpu(EXT4_SB(sb)->s_es->s_first_data_block)) {
		ext4_warning(sb, "won't resize using backup superblock at %llu",
			(unsigned long long)EXT4_SB(sb)->s_sbh->b_blocknr);
		return -EPERM;
	}

	gdb_bh = sb_bread(sb, gdblock);
	if (!gdb_bh)
		return -EIO;

	gdbackups = verify_reserved_gdb(sb, group, gdb_bh);
	if (gdbackups < 0) {
		err = gdbackups;
		goto exit_bh;
	}

	data = EXT4_I(inode)->i_data + EXT4_DIND_BLOCK;
	dind = sb_bread(sb, le32_to_cpu(*data));
	if (!dind) {
		err = -EIO;
		goto exit_bh;
	}

	data = (__le32 *)dind->b_data;
	if (le32_to_cpu(data[gdb_num % EXT4_ADDR_PER_BLOCK(sb)]) != gdblock) {
		ext4_warning(sb, "new group %u GDT block %llu not reserved",
			     group, gdblock);
		err = -EINVAL;
		goto exit_dind;
	}

	err = ext4_journal_get_write_access(handle, EXT4_SB(sb)->s_sbh);
	if (unlikely(err))
		goto exit_dind;

	err = ext4_journal_get_write_access(handle, gdb_bh);
	if (unlikely(err))
		goto exit_sbh;

	err = ext4_journal_get_write_access(handle, dind);
	if (unlikely(err))
		ext4_std_error(sb, err);

	/* ext4_reserve_inode_write() gets a reference on the iloc */
	err = ext4_reserve_inode_write(handle, inode, &iloc);
	if (unlikely(err))
		goto exit_dindj;

	n_group_desc = ext4_kvmalloc((gdb_num + 1) *
				     sizeof(struct buffer_head *),
				     GFP_NOFS);
	if (!n_group_desc) {
		err = -ENOMEM;
		ext4_warning(sb, "not enough memory for %lu groups",
			     gdb_num + 1);
		goto exit_inode;
	}

	/*
	 * Finally, we have all of the possible failures behind us...
	 *
	 * Remove new GDT block from inode double-indirect block and clear out
	 * the new GDT block for use (which also "frees" the backup GDT blocks
	 * from the reserved inode).  We don't need to change the bitmaps for
	 * these blocks, because they are marked as in-use from being in the
	 * reserved inode, and will become GDT blocks (primary and backup).
	 */
	data[gdb_num % EXT4_ADDR_PER_BLOCK(sb)] = 0;
	err = ext4_handle_dirty_metadata(handle, NULL, dind);
	if (unlikely(err)) {
		ext4_std_error(sb, err);
		goto exit_inode;
	}
	inode->i_blocks -= (gdbackups + 1) * sb->s_blocksize >> 9;
	ext4_mark_iloc_dirty(handle, inode, &iloc);
	memset(gdb_bh->b_data, 0, sb->s_blocksize);
	err = ext4_handle_dirty_metadata(handle, NULL, gdb_bh);
	if (unlikely(err)) {
		ext4_std_error(sb, err);
		goto exit_inode;
	}
	brelse(dind);

	o_group_desc = EXT4_SB(sb)->s_group_desc;
	memcpy(n_group_desc, o_group_desc,
	       EXT4_SB(sb)->s_gdb_count * sizeof(struct buffer_head *));
	n_group_desc[gdb_num] = gdb_bh;
	EXT4_SB(sb)->s_group_desc = n_group_desc;
	EXT4_SB(sb)->s_gdb_count++;
	ext4_kvfree(o_group_desc);

	le16_add_cpu(&es->s_reserved_gdt_blocks, -1);
	err = ext4_handle_dirty_super_now(handle, sb);
	if (err)
		ext4_std_error(sb, err);

	return err;

exit_inode:
	ext4_kvfree(n_group_desc);
	/* ext4_handle_release_buffer(handle, iloc.bh); */
	brelse(iloc.bh);
exit_dindj:
	/* ext4_handle_release_buffer(handle, dind); */
exit_sbh:
	/* ext4_handle_release_buffer(handle, EXT4_SB(sb)->s_sbh); */
exit_dind:
	brelse(dind);
exit_bh:
	brelse(gdb_bh);

	ext4_debug("leaving with error %d\n", err);
	return err;
}