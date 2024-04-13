int ext4_resize_fs(struct super_block *sb, ext4_fsblk_t n_blocks_count)
{
	struct ext4_new_flex_group_data *flex_gd = NULL;
	struct ext4_sb_info *sbi = EXT4_SB(sb);
	struct ext4_super_block *es = sbi->s_es;
	struct buffer_head *bh;
	struct inode *resize_inode;
	ext4_fsblk_t o_blocks_count;
	ext4_group_t o_group;
	ext4_group_t n_group;
	ext4_grpblk_t offset, add;
	unsigned long n_desc_blocks;
	unsigned long o_desc_blocks;
	unsigned long desc_blocks;
	int err = 0, flexbg_size = 1;

	o_blocks_count = ext4_blocks_count(es);

	if (test_opt(sb, DEBUG))
		ext4_msg(sb, KERN_DEBUG, "resizing filesystem from %llu "
		       "to %llu blocks", o_blocks_count, n_blocks_count);

	if (n_blocks_count < o_blocks_count) {
		/* On-line shrinking not supported */
		ext4_warning(sb, "can't shrink FS - resize aborted");
		return -EINVAL;
	}

	if (n_blocks_count == o_blocks_count)
		/* Nothing need to do */
		return 0;

	ext4_get_group_no_and_offset(sb, n_blocks_count - 1, &n_group, &offset);
	ext4_get_group_no_and_offset(sb, o_blocks_count - 1, &o_group, &offset);

	n_desc_blocks = (n_group + EXT4_DESC_PER_BLOCK(sb)) /
			EXT4_DESC_PER_BLOCK(sb);
	o_desc_blocks = (sbi->s_groups_count + EXT4_DESC_PER_BLOCK(sb) - 1) /
			EXT4_DESC_PER_BLOCK(sb);
	desc_blocks = n_desc_blocks - o_desc_blocks;

	if (desc_blocks &&
	    (!EXT4_HAS_COMPAT_FEATURE(sb, EXT4_FEATURE_COMPAT_RESIZE_INODE) ||
	     le16_to_cpu(es->s_reserved_gdt_blocks) < desc_blocks)) {
		ext4_warning(sb, "No reserved GDT blocks, can't resize");
		return -EPERM;
	}

	resize_inode = ext4_iget(sb, EXT4_RESIZE_INO);
	if (IS_ERR(resize_inode)) {
		ext4_warning(sb, "Error opening resize inode");
		return PTR_ERR(resize_inode);
	}

	/* See if the device is actually as big as what was requested */
	bh = sb_bread(sb, n_blocks_count - 1);
	if (!bh) {
		ext4_warning(sb, "can't read last block, resize aborted");
		return -ENOSPC;
	}
	brelse(bh);

	/* extend the last group */
	if (n_group == o_group)
		add = n_blocks_count - o_blocks_count;
	else
		add = EXT4_BLOCKS_PER_GROUP(sb) - (offset + 1);
	if (add > 0) {
		err = ext4_group_extend_no_check(sb, o_blocks_count, add);
		if (err)
			goto out;
	}

	if (EXT4_HAS_INCOMPAT_FEATURE(sb, EXT4_FEATURE_INCOMPAT_FLEX_BG) &&
	    es->s_log_groups_per_flex)
		flexbg_size = 1 << es->s_log_groups_per_flex;

	o_blocks_count = ext4_blocks_count(es);
	if (o_blocks_count == n_blocks_count)
		goto out;

	flex_gd = alloc_flex_gd(flexbg_size);
	if (flex_gd == NULL) {
		err = -ENOMEM;
		goto out;
	}

	/* Add flex groups. Note that a regular group is a
	 * flex group with 1 group.
	 */
	while (ext4_setup_next_flex_gd(sb, flex_gd, n_blocks_count,
					      flexbg_size)) {
		ext4_alloc_group_tables(sb, flex_gd, flexbg_size);
		err = ext4_flex_group_add(sb, resize_inode, flex_gd);
		if (unlikely(err))
			break;
	}

out:
	if (flex_gd)
		free_flex_gd(flex_gd);

	iput(resize_inode);
	if (test_opt(sb, DEBUG))
		ext4_msg(sb, KERN_DEBUG, "resized filesystem from %llu "
		       "upto %llu blocks", o_blocks_count, n_blocks_count);
	return err;
}