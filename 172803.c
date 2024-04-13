void ext4_mark_group_bitmap_corrupted(struct super_block *sb,
				     ext4_group_t group,
				     unsigned int flags)
{
	struct ext4_sb_info *sbi = EXT4_SB(sb);
	struct ext4_group_info *grp = ext4_get_group_info(sb, group);
	struct ext4_group_desc *gdp = ext4_get_group_desc(sb, group, NULL);
	int ret;

	if (flags & EXT4_GROUP_INFO_BBITMAP_CORRUPT) {
		ret = ext4_test_and_set_bit(EXT4_GROUP_INFO_BBITMAP_CORRUPT_BIT,
					    &grp->bb_state);
		if (!ret)
			percpu_counter_sub(&sbi->s_freeclusters_counter,
					   grp->bb_free);
	}

	if (flags & EXT4_GROUP_INFO_IBITMAP_CORRUPT) {
		ret = ext4_test_and_set_bit(EXT4_GROUP_INFO_IBITMAP_CORRUPT_BIT,
					    &grp->bb_state);
		if (!ret && gdp) {
			int count;

			count = ext4_free_inodes_count(sb, gdp);
			percpu_counter_sub(&sbi->s_freeinodes_counter,
					   count);
		}
	}
}