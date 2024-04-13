__timespec_to_jiffies(unsigned long sec, long nsec)
{
	return __timespec64_to_jiffies((u64)sec, nsec);
}