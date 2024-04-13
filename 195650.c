static int count_overhead(struct super_block *sb, ext4_group_t grp,
			  char *buf)
{
	struct ext4_sb_info	*sbi = EXT4_SB(sb);
	struct ext4_group_desc	*gdp;
	ext4_fsblk_t		first_block, last_block, b;
	ext4_group_t		i, ngroups = ext4_get_groups_count(sb);
	int			s, j, count = 0;

	first_block = le32_to_cpu(sbi->s_es->s_first_data_block) +
		(grp * EXT4_BLOCKS_PER_GROUP(sb));
	last_block = first_block + EXT4_BLOCKS_PER_GROUP(sb) - 1;
	for (i = 0; i < ngroups; i++) {
		gdp = ext4_get_group_desc(sb, i, NULL);
		b = ext4_block_bitmap(sb, gdp);
		if (b >= first_block && b <= last_block) {
			ext4_set_bit(EXT4_B2C(sbi, b - first_block), buf);
			count++;
		}
		b = ext4_inode_bitmap(sb, gdp);
		if (b >= first_block && b <= last_block) {
			ext4_set_bit(EXT4_B2C(sbi, b - first_block), buf);
			count++;
		}
		b = ext4_inode_table(sb, gdp);
		if (b >= first_block && b + sbi->s_itb_per_group <= last_block)
			for (j = 0; j < sbi->s_itb_per_group; j++, b++) {
				int c = EXT4_B2C(sbi, b - first_block);
				ext4_set_bit(c, buf);
				count++;
			}
		if (i != grp)
			continue;
		s = 0;
		if (ext4_bg_has_super(sb, grp)) {
			ext4_set_bit(s++, buf);
			count++;
		}
		for (j = ext4_bg_num_gdb(sb, grp); j > 0; j--) {
			ext4_set_bit(EXT4_B2C(sbi, s++), buf);
			count++;
		}
	}
	if (!count)
		return 0;
	return EXT4_CLUSTERS_PER_GROUP(sb) -
		ext4_count_free(buf, EXT4_CLUSTERS_PER_GROUP(sb) / 8);
}