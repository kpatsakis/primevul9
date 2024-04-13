static void __ext4_update_tstamp(__le32 *lo, __u8 *hi)
{
	time64_t now = ktime_get_real_seconds();

	now = clamp_val(now, 0, (1ull << 40) - 1);

	*lo = cpu_to_le32(lower_32_bits(now));
	*hi = upper_32_bits(now);
}