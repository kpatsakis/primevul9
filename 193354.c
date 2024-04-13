gfloat ves_icall_System_Threading_Interlocked_Exchange_Single (gfloat *location, gfloat value)
{
	IntFloatUnion val, ret;

	MONO_ARCH_SAVE_REGS;

	val.fval = value;
	ret.ival = InterlockedExchange((gint32 *) location, val.ival);

	return ret.fval;
}