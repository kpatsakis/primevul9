static ssize_t delayed_allocation_blocks_show(struct ext4_attr *a,
					      struct ext4_sb_info *sbi,
					      char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%llu\n",
		(s64) EXT4_C2B(sbi,
			percpu_counter_sum(&sbi->s_dirtyclusters_counter)));
}