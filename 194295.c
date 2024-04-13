static inline s16 fixp_new16(s16 a)
{
	return ((s32)a) >> (16 - FRAC_N);
}