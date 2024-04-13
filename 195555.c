static int verify_group_input(struct super_block *sb,
			      struct ext4_new_group_data *input)
{
	struct ext4_sb_info *sbi = EXT4_SB(sb);
	struct ext4_super_block *es = sbi->s_es;
	ext4_fsblk_t start = ext4_blocks_count(es);
	ext4_fsblk_t end = start + input->blocks_count;
	ext4_group_t group = input->group;
	ext4_fsblk_t itend = input->inode_table + sbi->s_itb_per_group;
	unsigned overhead = ext4_bg_has_super(sb, group) ?
		(1 + ext4_bg_num_gdb(sb, group) +
		 le16_to_cpu(es->s_reserved_gdt_blocks)) : 0;
	ext4_fsblk_t metaend = start + overhead;
	struct buffer_head *bh = NULL;
	ext4_grpblk_t free_blocks_count, offset;
	int err = -EINVAL;

	input->free_blocks_count = free_blocks_count =
		input->blocks_count - 2 - overhead - sbi->s_itb_per_group;

	if (test_opt(sb, DEBUG))
		printk(KERN_DEBUG "EXT4-fs: adding %s group %u: %u blocks "
		       "(%d free, %u reserved)\n",
		       ext4_bg_has_super(sb, input->group) ? "normal" :
		       "no-super", input->group, input->blocks_count,
		       free_blocks_count, input->reserved_blocks);

	ext4_get_group_no_and_offset(sb, start, NULL, &offset);
	if (group != sbi->s_groups_count)
		ext4_warning(sb, "Cannot add at group %u (only %u groups)",
			     input->group, sbi->s_groups_count);
	else if (offset != 0)
			ext4_warning(sb, "Last group not full");
	else if (input->reserved_blocks > input->blocks_count / 5)
		ext4_warning(sb, "Reserved blocks too high (%u)",
			     input->reserved_blocks);
	else if (free_blocks_count < 0)
		ext4_warning(sb, "Bad blocks count %u",
			     input->blocks_count);
	else if (!(bh = sb_bread(sb, end - 1)))
		ext4_warning(sb, "Cannot read last block (%llu)",
			     end - 1);
	else if (outside(input->block_bitmap, start, end))
		ext4_warning(sb, "Block bitmap not in group (block %llu)",
			     (unsigned long long)input->block_bitmap);
	else if (outside(input->inode_bitmap, start, end))
		ext4_warning(sb, "Inode bitmap not in group (block %llu)",
			     (unsigned long long)input->inode_bitmap);
	else if (outside(input->inode_table, start, end) ||
		 outside(itend - 1, start, end))
		ext4_warning(sb, "Inode table not in group (blocks %llu-%llu)",
			     (unsigned long long)input->inode_table, itend - 1);
	else if (input->inode_bitmap == input->block_bitmap)
		ext4_warning(sb, "Block bitmap same as inode bitmap (%llu)",
			     (unsigned long long)input->block_bitmap);
	else if (inside(input->block_bitmap, input->inode_table, itend))
		ext4_warning(sb, "Block bitmap (%llu) in inode table "
			     "(%llu-%llu)",
			     (unsigned long long)input->block_bitmap,
			     (unsigned long long)input->inode_table, itend - 1);
	else if (inside(input->inode_bitmap, input->inode_table, itend))
		ext4_warning(sb, "Inode bitmap (%llu) in inode table "
			     "(%llu-%llu)",
			     (unsigned long long)input->inode_bitmap,
			     (unsigned long long)input->inode_table, itend - 1);
	else if (inside(input->block_bitmap, start, metaend))
		ext4_warning(sb, "Block bitmap (%llu) in GDT table (%llu-%llu)",
			     (unsigned long long)input->block_bitmap,
			     start, metaend - 1);
	else if (inside(input->inode_bitmap, start, metaend))
		ext4_warning(sb, "Inode bitmap (%llu) in GDT table (%llu-%llu)",
			     (unsigned long long)input->inode_bitmap,
			     start, metaend - 1);
	else if (inside(input->inode_table, start, metaend) ||
		 inside(itend - 1, start, metaend))
		ext4_warning(sb, "Inode table (%llu-%llu) overlaps GDT table "
			     "(%llu-%llu)",
			     (unsigned long long)input->inode_table,
			     itend - 1, start, metaend - 1);
	else
		err = 0;
	brelse(bh);

	return err;
}