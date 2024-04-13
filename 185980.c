static int fiemap_check_ranges(struct super_block *sb,
			       u64 start, u64 len, u64 *new_len)
{
	u64 maxbytes = (u64) sb->s_maxbytes;

	*new_len = len;

	if (len == 0)
		return -EINVAL;

	if (start > maxbytes)
		return -EFBIG;

	/*
	 * Shrink request scope to what the fs can actually handle.
	 */
	if (len > maxbytes || (maxbytes - len) < start)
		*new_len = maxbytes - start;

	return 0;
}