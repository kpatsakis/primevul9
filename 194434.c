static inline u64 xgetbv(u32 index)
{
	u32 eax, edx;

	asm volatile(".byte 0x0f,0x01,0xd0" /* xgetbv */
		     : "=a" (eax), "=d" (edx)
		     : "c" (index));
	return eax + ((u64)edx << 32);
}