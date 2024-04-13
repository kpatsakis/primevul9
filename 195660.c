int ext4_group_extend(struct super_block *sb, struct ext4_super_block *es,
		      ext4_fsblk_t n_blocks_count)
{
	ext4_fsblk_t o_blocks_count;
	ext4_grpblk_t last;
	ext4_grpblk_t add;
	struct buffer_head *bh;
	int err;
	ext4_group_t group;

	o_blocks_count = ext4_blocks_count(es);

	if (test_opt(sb, DEBUG))
		ext4_msg(sb, KERN_DEBUG,
			 "extending last group from %llu to %llu blocks",
			 o_blocks_count, n_blocks_count);

	if (n_blocks_count == 0 || n_blocks_count == o_blocks_count)
		return 0;

	if (n_blocks_count > (sector_t)(~0ULL) >> (sb->s_blocksize_bits - 9)) {
		ext4_msg(sb, KERN_ERR,
			 "filesystem too large to resize to %llu blocks safely",
			 n_blocks_count);
		if (sizeof(sector_t) < 8)
			ext4_warning(sb, "CONFIG_LBDAF not enabled");
		return -EINVAL;
	}

	if (n_blocks_count < o_blocks_count) {
		ext4_warning(sb, "can't shrink FS - resize aborted");
		return -EINVAL;
	}

	/* Handle the remaining blocks in the last group only. */
	ext4_get_group_no_and_offset(sb, o_blocks_count, &group, &last);

	if (last == 0) {
		ext4_warning(sb, "need to use ext2online to resize further");
		return -EPERM;
	}

	add = EXT4_BLOCKS_PER_GROUP(sb) - last;

	if (o_blocks_count + add < o_blocks_count) {
		ext4_warning(sb, "blocks_count overflow");
		return -EINVAL;
	}

	if (o_blocks_count + add > n_blocks_count)
		add = n_blocks_count - o_blocks_count;

	if (o_blocks_count + add < n_blocks_count)
		ext4_warning(sb, "will only finish group (%llu blocks, %u new)",
			     o_blocks_count + add, add);

	/* See if the device is actually as big as what was requested */
	bh = sb_bread(sb, o_blocks_count + add - 1);
	if (!bh) {
		ext4_warning(sb, "can't read last block, resize aborted");
		return -ENOSPC;
	}
	brelse(bh);

	err = ext4_group_extend_no_check(sb, o_blocks_count, add);
	return err;
} /* ext4_group_extend */