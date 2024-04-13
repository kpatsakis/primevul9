static int ext4_add_new_descs(handle_t *handle, struct super_block *sb,
			      ext4_group_t group, struct inode *resize_inode,
			      ext4_group_t count)
{
	struct ext4_sb_info *sbi = EXT4_SB(sb);
	struct ext4_super_block *es = sbi->s_es;
	struct buffer_head *gdb_bh;
	int i, gdb_off, gdb_num, err = 0;

	for (i = 0; i < count; i++, group++) {
		int reserved_gdb = ext4_bg_has_super(sb, group) ?
			le16_to_cpu(es->s_reserved_gdt_blocks) : 0;

		gdb_off = group % EXT4_DESC_PER_BLOCK(sb);
		gdb_num = group / EXT4_DESC_PER_BLOCK(sb);

		/*
		 * We will only either add reserved group blocks to a backup group
		 * or remove reserved blocks for the first group in a new group block.
		 * Doing both would be mean more complex code, and sane people don't
		 * use non-sparse filesystems anymore.  This is already checked above.
		 */
		if (gdb_off) {
			gdb_bh = sbi->s_group_desc[gdb_num];
			err = ext4_journal_get_write_access(handle, gdb_bh);

			if (!err && reserved_gdb && ext4_bg_num_gdb(sb, group))
				err = reserve_backup_gdb(handle, resize_inode, group);
		} else
			err = add_new_gdb(handle, resize_inode, group);
		if (err)
			break;
	}
	return err;
}