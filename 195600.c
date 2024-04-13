static int set_flexbg_block_bitmap(struct super_block *sb, handle_t *handle,
			struct ext4_new_flex_group_data *flex_gd,
			ext4_fsblk_t block, ext4_group_t count)
{
	ext4_group_t count2;

	ext4_debug("mark blocks [%llu/%u] used\n", block, count);
	for (count2 = count; count > 0; count -= count2, block += count2) {
		ext4_fsblk_t start;
		struct buffer_head *bh;
		ext4_group_t group;
		int err;

		ext4_get_group_no_and_offset(sb, block, &group, NULL);
		start = ext4_group_first_block_no(sb, group);
		group -= flex_gd->groups[0].group;

		count2 = sb->s_blocksize * 8 - (block - start);
		if (count2 > count)
			count2 = count;

		if (flex_gd->bg_flags[group] & EXT4_BG_BLOCK_UNINIT) {
			BUG_ON(flex_gd->count > 1);
			continue;
		}

		err = extend_or_restart_transaction(handle, 1);
		if (err)
			return err;

		bh = sb_getblk(sb, flex_gd->groups[group].block_bitmap);
		if (!bh)
			return -EIO;

		err = ext4_journal_get_write_access(handle, bh);
		if (err)
			return err;
		ext4_debug("mark block bitmap %#04llx (+%llu/%u)\n", block,
			   block - start, count2);
		ext4_set_bits(bh->b_data, block - start, count2);

		err = ext4_handle_dirty_metadata(handle, NULL, bh);
		if (unlikely(err))
			return err;
		brelse(bh);
	}

	return 0;
}