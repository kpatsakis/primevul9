static time64_t __ext4_get_tstamp(__le32 *lo, __u8 *hi)
{
	return ((time64_t)(*hi) << 32) + le32_to_cpu(*lo);
}