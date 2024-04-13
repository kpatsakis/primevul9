static inline void *alloc_smp_resp(int size)
{
	return kzalloc(size, GFP_KERNEL);
}