static int setup_new_flex_group_blocks(struct super_block *sb,
				struct ext4_new_flex_group_data *flex_gd)
{
	int group_table_count[] = {1, 1, EXT4_SB(sb)->s_itb_per_group};
	ext4_fsblk_t start;
	ext4_fsblk_t block;
	struct ext4_sb_info *sbi = EXT4_SB(sb);
	struct ext4_super_block *es = sbi->s_es;
	struct ext4_new_group_data *group_data = flex_gd->groups;
	__u16 *bg_flags = flex_gd->bg_flags;
	handle_t *handle;
	ext4_group_t group, count;
	struct buffer_head *bh = NULL;
	int reserved_gdb, i, j, err = 0, err2;

	BUG_ON(!flex_gd->count || !group_data ||
	       group_data[0].group != sbi->s_groups_count);

	reserved_gdb = le16_to_cpu(es->s_reserved_gdt_blocks);

	/* This transaction may be extended/restarted along the way */
	handle = ext4_journal_start_sb(sb, EXT4_MAX_TRANS_DATA);
	if (IS_ERR(handle))
		return PTR_ERR(handle);

	group = group_data[0].group;
	for (i = 0; i < flex_gd->count; i++, group++) {
		unsigned long gdblocks;

		gdblocks = ext4_bg_num_gdb(sb, group);
		start = ext4_group_first_block_no(sb, group);

		/* Copy all of the GDT blocks into the backup in this group */
		for (j = 0, block = start + 1; j < gdblocks; j++, block++) {
			struct buffer_head *gdb;

			ext4_debug("update backup group %#04llx\n", block);
			err = extend_or_restart_transaction(handle, 1);
			if (err)
				goto out;

			gdb = sb_getblk(sb, block);
			if (!gdb) {
				err = -EIO;
				goto out;
			}

			err = ext4_journal_get_write_access(handle, gdb);
			if (err) {
				brelse(gdb);
				goto out;
			}
			memcpy(gdb->b_data, sbi->s_group_desc[j]->b_data,
			       gdb->b_size);
			set_buffer_uptodate(gdb);

			err = ext4_handle_dirty_metadata(handle, NULL, gdb);
			if (unlikely(err)) {
				brelse(gdb);
				goto out;
			}
			brelse(gdb);
		}

		/* Zero out all of the reserved backup group descriptor
		 * table blocks
		 */
		if (ext4_bg_has_super(sb, group)) {
			err = sb_issue_zeroout(sb, gdblocks + start + 1,
					reserved_gdb, GFP_NOFS);
			if (err)
				goto out;
		}

		/* Initialize group tables of the grop @group */
		if (!(bg_flags[i] & EXT4_BG_INODE_ZEROED))
			goto handle_bb;

		/* Zero out all of the inode table blocks */
		block = group_data[i].inode_table;
		ext4_debug("clear inode table blocks %#04llx -> %#04lx\n",
			   block, sbi->s_itb_per_group);
		err = sb_issue_zeroout(sb, block, sbi->s_itb_per_group,
				       GFP_NOFS);
		if (err)
			goto out;

handle_bb:
		if (bg_flags[i] & EXT4_BG_BLOCK_UNINIT)
			goto handle_ib;

		/* Initialize block bitmap of the @group */
		block = group_data[i].block_bitmap;
		err = extend_or_restart_transaction(handle, 1);
		if (err)
			goto out;

		bh = bclean(handle, sb, block);
		if (IS_ERR(bh)) {
			err = PTR_ERR(bh);
			goto out;
		}
		if (ext4_bg_has_super(sb, group)) {
			ext4_debug("mark backup superblock %#04llx (+0)\n",
				   start);
			ext4_set_bits(bh->b_data, 0, gdblocks + reserved_gdb +
						     1);
		}
		ext4_mark_bitmap_end(group_data[i].blocks_count,
				     sb->s_blocksize * 8, bh->b_data);
		err = ext4_handle_dirty_metadata(handle, NULL, bh);
		if (err)
			goto out;
		brelse(bh);

handle_ib:
		if (bg_flags[i] & EXT4_BG_INODE_UNINIT)
			continue;

		/* Initialize inode bitmap of the @group */
		block = group_data[i].inode_bitmap;
		err = extend_or_restart_transaction(handle, 1);
		if (err)
			goto out;
		/* Mark unused entries in inode bitmap used */
		bh = bclean(handle, sb, block);
		if (IS_ERR(bh)) {
			err = PTR_ERR(bh);
			goto out;
		}

		ext4_mark_bitmap_end(EXT4_INODES_PER_GROUP(sb),
				     sb->s_blocksize * 8, bh->b_data);
		err = ext4_handle_dirty_metadata(handle, NULL, bh);
		if (err)
			goto out;
		brelse(bh);
	}
	bh = NULL;

	/* Mark group tables in block bitmap */
	for (j = 0; j < GROUP_TABLE_COUNT; j++) {
		count = group_table_count[j];
		start = (&group_data[0].block_bitmap)[j];
		block = start;
		for (i = 1; i < flex_gd->count; i++) {
			block += group_table_count[j];
			if (block == (&group_data[i].block_bitmap)[j]) {
				count += group_table_count[j];
				continue;
			}
			err = set_flexbg_block_bitmap(sb, handle,
						flex_gd, start, count);
			if (err)
				goto out;
			count = group_table_count[j];
			start = group_data[i].block_bitmap;
			block = start;
		}

		if (count) {
			err = set_flexbg_block_bitmap(sb, handle,
						flex_gd, start, count);
			if (err)
				goto out;
		}
	}

out:
	brelse(bh);
	err2 = ext4_journal_stop(handle);
	if (err2 && !err)
		err = err2;

	return err;
}