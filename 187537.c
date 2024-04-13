static inline void *alloc_smp_req(int size)
{
	u8 *p = kzalloc(size, GFP_KERNEL);
	if (p)
		p[0] = SMP_REQUEST;
	return p;
}