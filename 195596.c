static int ext4_flex_group_add(struct super_block *sb,
			       struct inode *resize_inode,
			       struct ext4_new_flex_group_data *flex_gd)
{
	struct ext4_sb_info *sbi = EXT4_SB(sb);
	struct ext4_super_block *es = sbi->s_es;
	ext4_fsblk_t o_blocks_count;
	ext4_grpblk_t last;
	ext4_group_t group;
	handle_t *handle;
	unsigned reserved_gdb;
	int err = 0, err2 = 0, credit;

	BUG_ON(!flex_gd->count || !flex_gd->groups || !flex_gd->bg_flags);

	reserved_gdb = le16_to_cpu(es->s_reserved_gdt_blocks);
	o_blocks_count = ext4_blocks_count(es);
	ext4_get_group_no_and_offset(sb, o_blocks_count, &group, &last);
	BUG_ON(last);

	err = setup_new_flex_group_blocks(sb, flex_gd);
	if (err)
		goto exit;
	/*
	 * We will always be modifying at least the superblock and  GDT
	 * block.  If we are adding a group past the last current GDT block,
	 * we will also modify the inode and the dindirect block.  If we
	 * are adding a group with superblock/GDT backups  we will also
	 * modify each of the reserved GDT dindirect blocks.
	 */
	credit = flex_gd->count * 4 + reserved_gdb;
	handle = ext4_journal_start_sb(sb, credit);
	if (IS_ERR(handle)) {
		err = PTR_ERR(handle);
		goto exit;
	}

	err = ext4_journal_get_write_access(handle, sbi->s_sbh);
	if (err)
		goto exit_journal;

	group = flex_gd->groups[0].group;
	BUG_ON(group != EXT4_SB(sb)->s_groups_count);
	err = ext4_add_new_descs(handle, sb, group,
				resize_inode, flex_gd->count);
	if (err)
		goto exit_journal;

	err = ext4_setup_new_descs(handle, sb, flex_gd);
	if (err)
		goto exit_journal;

	ext4_update_super(sb, flex_gd);

	err = ext4_handle_dirty_super(handle, sb);

exit_journal:
	err2 = ext4_journal_stop(handle);
	if (!err)
		err = err2;

	if (!err) {
		int i;
		update_backups(sb, sbi->s_sbh->b_blocknr, (char *)es,
			       sizeof(struct ext4_super_block));
		for (i = 0; i < flex_gd->count; i++, group++) {
			struct buffer_head *gdb_bh;
			int gdb_num;
			gdb_num = group / EXT4_BLOCKS_PER_GROUP(sb);
			gdb_bh = sbi->s_group_desc[gdb_num];
			update_backups(sb, gdb_bh->b_blocknr, gdb_bh->b_data,
				       gdb_bh->b_size);
		}
	}
exit:
	return err;
}