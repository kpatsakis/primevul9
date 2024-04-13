static int ext4_group_extend_no_check(struct super_block *sb,
				      ext4_fsblk_t o_blocks_count, ext4_grpblk_t add)
{
	struct ext4_super_block *es = EXT4_SB(sb)->s_es;
	handle_t *handle;
	int err = 0, err2;

	/* We will update the superblock, one block bitmap, and
	 * one group descriptor via ext4_group_add_blocks().
	 */
	handle = ext4_journal_start_sb(sb, 3);
	if (IS_ERR(handle)) {
		err = PTR_ERR(handle);
		ext4_warning(sb, "error %d on journal start", err);
		return err;
	}

	err = ext4_journal_get_write_access(handle, EXT4_SB(sb)->s_sbh);
	if (err) {
		ext4_warning(sb, "error %d on journal write access", err);
		goto errout;
	}

	ext4_blocks_count_set(es, o_blocks_count + add);
	ext4_free_blocks_count_set(es, ext4_free_blocks_count(es) + add);
	ext4_debug("freeing blocks %llu through %llu\n", o_blocks_count,
		   o_blocks_count + add);
	/* We add the blocks to the bitmap and set the group need init bit */
	err = ext4_group_add_blocks(handle, sb, o_blocks_count, add);
	if (err)
		goto errout;
	ext4_handle_dirty_super(handle, sb);
	ext4_debug("freed blocks %llu through %llu\n", o_blocks_count,
		   o_blocks_count + add);
errout:
	err2 = ext4_journal_stop(handle);
	if (err2 && !err)
		err = err2;

	if (!err) {
		if (test_opt(sb, DEBUG))
			printk(KERN_DEBUG "EXT4-fs: extended group to %llu "
			       "blocks\n", ext4_blocks_count(es));
		update_backups(sb, EXT4_SB(sb)->s_sbh->b_blocknr, (char *)es,
			       sizeof(struct ext4_super_block));
	}
	return err;
}