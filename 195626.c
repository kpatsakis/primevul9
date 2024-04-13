static void ext4_alloc_group_tables(struct super_block *sb,
				struct ext4_new_flex_group_data *flex_gd,
				int flexbg_size)
{
	struct ext4_new_group_data *group_data = flex_gd->groups;
	struct ext4_super_block *es = EXT4_SB(sb)->s_es;
	ext4_fsblk_t start_blk;
	ext4_fsblk_t last_blk;
	ext4_group_t src_group;
	ext4_group_t bb_index = 0;
	ext4_group_t ib_index = 0;
	ext4_group_t it_index = 0;
	ext4_group_t group;
	ext4_group_t last_group;
	unsigned overhead;

	BUG_ON(flex_gd->count == 0 || group_data == NULL);

	src_group = group_data[0].group;
	last_group  = src_group + flex_gd->count - 1;

	BUG_ON((flexbg_size > 1) && ((src_group & ~(flexbg_size - 1)) !=
	       (last_group & ~(flexbg_size - 1))));
next_group:
	group = group_data[0].group;
	start_blk = ext4_group_first_block_no(sb, src_group);
	last_blk = start_blk + group_data[src_group - group].blocks_count;

	overhead = ext4_bg_has_super(sb, src_group) ?
		   (1 + ext4_bg_num_gdb(sb, src_group) +
		    le16_to_cpu(es->s_reserved_gdt_blocks)) : 0;

	start_blk += overhead;

	BUG_ON(src_group >= group_data[0].group + flex_gd->count);
	/* We collect contiguous blocks as much as possible. */
	src_group++;
	for (; src_group <= last_group; src_group++)
		if (!ext4_bg_has_super(sb, src_group))
			last_blk += group_data[src_group - group].blocks_count;
		else
			break;

	/* Allocate block bitmaps */
	for (; bb_index < flex_gd->count; bb_index++) {
		if (start_blk >= last_blk)
			goto next_group;
		group_data[bb_index].block_bitmap = start_blk++;
		ext4_get_group_no_and_offset(sb, start_blk - 1, &group, NULL);
		group -= group_data[0].group;
		group_data[group].free_blocks_count--;
		if (flexbg_size > 1)
			flex_gd->bg_flags[group] &= ~EXT4_BG_BLOCK_UNINIT;
	}

	/* Allocate inode bitmaps */
	for (; ib_index < flex_gd->count; ib_index++) {
		if (start_blk >= last_blk)
			goto next_group;
		group_data[ib_index].inode_bitmap = start_blk++;
		ext4_get_group_no_and_offset(sb, start_blk - 1, &group, NULL);
		group -= group_data[0].group;
		group_data[group].free_blocks_count--;
		if (flexbg_size > 1)
			flex_gd->bg_flags[group] &= ~EXT4_BG_BLOCK_UNINIT;
	}

	/* Allocate inode tables */
	for (; it_index < flex_gd->count; it_index++) {
		if (start_blk + EXT4_SB(sb)->s_itb_per_group > last_blk)
			goto next_group;
		group_data[it_index].inode_table = start_blk;
		ext4_get_group_no_and_offset(sb, start_blk, &group, NULL);
		group -= group_data[0].group;
		group_data[group].free_blocks_count -=
					EXT4_SB(sb)->s_itb_per_group;
		if (flexbg_size > 1)
			flex_gd->bg_flags[group] &= ~EXT4_BG_BLOCK_UNINIT;

		start_blk += EXT4_SB(sb)->s_itb_per_group;
	}

	if (test_opt(sb, DEBUG)) {
		int i;
		group = group_data[0].group;

		printk(KERN_DEBUG "EXT4-fs: adding a flex group with "
		       "%d groups, flexbg size is %d:\n", flex_gd->count,
		       flexbg_size);

		for (i = 0; i < flex_gd->count; i++) {
			printk(KERN_DEBUG "adding %s group %u: %u "
			       "blocks (%d free)\n",
			       ext4_bg_has_super(sb, group + i) ? "normal" :
			       "no-super", group + i,
			       group_data[i].blocks_count,
			       group_data[i].free_blocks_count);
		}
	}
}