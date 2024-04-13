static int jpc_calcssexp(jpc_fix_t stepsize)
{
	return jpc_fix_firstone(stepsize) - JPC_FIX_FRACBITS;
}