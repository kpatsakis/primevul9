static int ext4_setup_next_flex_gd(struct super_block *sb,
				    struct ext4_new_flex_group_data *flex_gd,
				    ext4_fsblk_t n_blocks_count,
				    unsigned long flexbg_size)
{
	struct ext4_super_block *es = EXT4_SB(sb)->s_es;
	struct ext4_new_group_data *group_data = flex_gd->groups;
	ext4_fsblk_t o_blocks_count;
	ext4_group_t n_group;
	ext4_group_t group;
	ext4_group_t last_group;
	ext4_grpblk_t last;
	ext4_grpblk_t blocks_per_group;
	unsigned long i;

	blocks_per_group = EXT4_BLOCKS_PER_GROUP(sb);

	o_blocks_count = ext4_blocks_count(es);

	if (o_blocks_count == n_blocks_count)
		return 0;

	ext4_get_group_no_and_offset(sb, o_blocks_count, &group, &last);
	BUG_ON(last);
	ext4_get_group_no_and_offset(sb, n_blocks_count - 1, &n_group, &last);

	last_group = group | (flexbg_size - 1);
	if (last_group > n_group)
		last_group = n_group;

	flex_gd->count = last_group - group + 1;

	for (i = 0; i < flex_gd->count; i++) {
		int overhead;

		group_data[i].group = group + i;
		group_data[i].blocks_count = blocks_per_group;
		overhead = ext4_bg_has_super(sb, group + i) ?
			   (1 + ext4_bg_num_gdb(sb, group + i) +
			    le16_to_cpu(es->s_reserved_gdt_blocks)) : 0;
		group_data[i].free_blocks_count = blocks_per_group - overhead;
		if (ext4_has_group_desc_csum(sb))
			flex_gd->bg_flags[i] = EXT4_BG_BLOCK_UNINIT |
					       EXT4_BG_INODE_UNINIT;
		else
			flex_gd->bg_flags[i] = EXT4_BG_INODE_ZEROED;
	}

	if (last_group == n_group && ext4_has_group_desc_csum(sb))
		/* We need to initialize block bitmap of last group. */
		flex_gd->bg_flags[i - 1] &= ~EXT4_BG_BLOCK_UNINIT;

	if ((last_group == n_group) && (last != blocks_per_group - 1)) {
		group_data[i - 1].blocks_count = last + 1;
		group_data[i - 1].free_blocks_count -= blocks_per_group-
					last - 1;
	}

	return 1;
}