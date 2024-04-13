static inline bool handle_valid(unsigned long handle)
{
	return !!(handle & ~0xff);
}