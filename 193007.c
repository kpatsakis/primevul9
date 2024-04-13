u64 jiffies64_to_nsecs(u64 j)
{
#if !(NSEC_PER_SEC % HZ)
	return (NSEC_PER_SEC / HZ) * j;
# else
	return div_u64(j * HZ_TO_NSEC_NUM, HZ_TO_NSEC_DEN);
#endif
}