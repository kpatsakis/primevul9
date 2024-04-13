static ssize_t trigger_test_error(struct ext4_attr *a,
				  struct ext4_sb_info *sbi,
				  const char *buf, size_t count)
{
	int len = count;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	if (len && buf[len-1] == '\n')
		len--;

	if (len)
		ext4_error(sbi->s_sb, "%.*s", len, buf);
	return count;
}