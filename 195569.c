static void ext4_update_super(struct super_block *sb,
			     struct ext4_new_flex_group_data *flex_gd)
{
	ext4_fsblk_t blocks_count = 0;
	ext4_fsblk_t free_blocks = 0;
	ext4_fsblk_t reserved_blocks = 0;
	struct ext4_new_group_data *group_data = flex_gd->groups;
	struct ext4_sb_info *sbi = EXT4_SB(sb);
	struct ext4_super_block *es = sbi->s_es;
	int i, ret;

	BUG_ON(flex_gd->count == 0 || group_data == NULL);
	/*
	 * Make the new blocks and inodes valid next.  We do this before
	 * increasing the group count so that once the group is enabled,
	 * all of its blocks and inodes are already valid.
	 *
	 * We always allocate group-by-group, then block-by-block or
	 * inode-by-inode within a group, so enabling these
	 * blocks/inodes before the group is live won't actually let us
	 * allocate the new space yet.
	 */
	for (i = 0; i < flex_gd->count; i++) {
		blocks_count += group_data[i].blocks_count;
		free_blocks += group_data[i].free_blocks_count;
	}

	reserved_blocks = ext4_r_blocks_count(es) * 100;
	do_div(reserved_blocks, ext4_blocks_count(es));
	reserved_blocks *= blocks_count;
	do_div(reserved_blocks, 100);

	ext4_blocks_count_set(es, ext4_blocks_count(es) + blocks_count);
	ext4_free_blocks_count_set(es, ext4_free_blocks_count(es) + free_blocks);
	le32_add_cpu(&es->s_inodes_count, EXT4_INODES_PER_GROUP(sb) *
		     flex_gd->count);
	le32_add_cpu(&es->s_free_inodes_count, EXT4_INODES_PER_GROUP(sb) *
		     flex_gd->count);

	/*
	 * We need to protect s_groups_count against other CPUs seeing
	 * inconsistent state in the superblock.
	 *
	 * The precise rules we use are:
	 *
	 * * Writers must perform a smp_wmb() after updating all
	 *   dependent data and before modifying the groups count
	 *
	 * * Readers must perform an smp_rmb() after reading the groups
	 *   count and before reading any dependent data.
	 *
	 * NB. These rules can be relaxed when checking the group count
	 * while freeing data, as we can only allocate from a block
	 * group after serialising against the group count, and we can
	 * only then free after serialising in turn against that
	 * allocation.
	 */
	smp_wmb();

	/* Update the global fs size fields */
	sbi->s_groups_count += flex_gd->count;

	/* Update the reserved block counts only once the new group is
	 * active. */
	ext4_r_blocks_count_set(es, ext4_r_blocks_count(es) +
				reserved_blocks);

	/* Update the free space counts */
	percpu_counter_add(&sbi->s_freeclusters_counter,
			   EXT4_B2C(sbi, free_blocks));
	percpu_counter_add(&sbi->s_freeinodes_counter,
			   EXT4_INODES_PER_GROUP(sb) * flex_gd->count);

	if (EXT4_HAS_INCOMPAT_FEATURE(sb,
				      EXT4_FEATURE_INCOMPAT_FLEX_BG) &&
	    sbi->s_log_groups_per_flex) {
		ext4_group_t flex_group;
		flex_group = ext4_flex_group(sbi, group_data[0].group);
		atomic_add(EXT4_B2C(sbi, free_blocks),
			   &sbi->s_flex_groups[flex_group].free_clusters);
		atomic_add(EXT4_INODES_PER_GROUP(sb) * flex_gd->count,
			   &sbi->s_flex_groups[flex_group].free_inodes);
	}

	/*
	 * Update the fs overhead information
	 */
	ext4_calculate_overhead(sb);

	if (test_opt(sb, DEBUG))
		printk(KERN_DEBUG "EXT4-fs: added group %u:"
		       "%llu blocks(%llu free %llu reserved)\n", flex_gd->count,
		       blocks_count, free_blocks, reserved_blocks);
}