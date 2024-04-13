static inline unsigned long ioctl32_hash(unsigned long cmd)
{
	return (((cmd >> 6) ^ (cmd >> 4) ^ cmd)) % IOCTL_HASHSIZE;
}