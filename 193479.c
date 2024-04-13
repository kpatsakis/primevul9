gfloat ves_icall_System_Threading_Interlocked_CompareExchange_Single (gfloat *location, gfloat value, gfloat comparand)
{
	IntFloatUnion val, ret, cmp;

	MONO_ARCH_SAVE_REGS;

	val.fval = value;
	cmp.fval = comparand;
	ret.ival = InterlockedCompareExchange((gint32 *) location, val.ival, cmp.ival);

	return ret.fval;
}