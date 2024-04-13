unsigned long nsecs_to_jiffies(u64 n)
{
	return (unsigned long)nsecs_to_jiffies64(n);
}