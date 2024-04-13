static const char *nfsd4_op_name(unsigned opnum)
{
	if (opnum < ARRAY_SIZE(nfsd4_ops))
		return nfsd4_ops[opnum].op_name;
	return "unknown_operation";
}