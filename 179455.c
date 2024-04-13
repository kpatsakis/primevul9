static inline unsigned int unmask_flags(unsigned long p)
{
	return p & ~PAGE_MASK;
}