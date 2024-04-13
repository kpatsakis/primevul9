static inline s16 fixp_mult(s16 a, s16 b)
{
	a = ((s32)a * 0x100) / 0x7fff;
	return ((s32)(a * b)) >> FRAC_N;
}