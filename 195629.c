static ssize_t inode_readahead_blks_store(struct ext4_attr *a,
					  struct ext4_sb_info *sbi,
					  const char *buf, size_t count)
{
	unsigned long t;

	if (parse_strtoul(buf, 0x40000000, &t))
		return -EINVAL;

	if (t && !is_power_of_2(t))
		return -EINVAL;

	sbi->s_inode_readahead_blks = t;
	return count;
}