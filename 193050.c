unsigned int jiffies_to_usecs(const unsigned long j)
{
	/*
	 * Hz usually doesn't go much further MSEC_PER_SEC.
	 * jiffies_to_usecs() and usecs_to_jiffies() depend on that.
	 */
	BUILD_BUG_ON(HZ > USEC_PER_SEC);

#if !(USEC_PER_SEC % HZ)
	return (USEC_PER_SEC / HZ) * j;
#else
# if BITS_PER_LONG == 32
	return (HZ_TO_USEC_MUL32 * j) >> HZ_TO_USEC_SHR32;
# else
	return (j * HZ_TO_USEC_NUM) / HZ_TO_USEC_DEN;
# endif
#endif
}