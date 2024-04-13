int ext4_calculate_overhead(struct super_block *sb)
{
	struct ext4_sb_info *sbi = EXT4_SB(sb);
	struct ext4_super_block *es = sbi->s_es;
	ext4_group_t i, ngroups = ext4_get_groups_count(sb);
	ext4_fsblk_t overhead = 0;
	char *buf = (char *) get_zeroed_page(GFP_KERNEL);

	memset(buf, 0, PAGE_SIZE);
	if (!buf)
		return -ENOMEM;

	/*
	 * Compute the overhead (FS structures).  This is constant
	 * for a given filesystem unless the number of block groups
	 * changes so we cache the previous value until it does.
	 */

	/*
	 * All of the blocks before first_data_block are overhead
	 */
	overhead = EXT4_B2C(sbi, le32_to_cpu(es->s_first_data_block));

	/*
	 * Add the overhead found in each block group
	 */
	for (i = 0; i < ngroups; i++) {
		int blks;

		blks = count_overhead(sb, i, buf);
		overhead += blks;
		if (blks)
			memset(buf, 0, PAGE_SIZE);
		cond_resched();
	}
	sbi->s_overhead = overhead;
	smp_wmb();
	free_page((unsigned long) buf);
	return 0;
}