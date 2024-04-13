static __be32 nfs41_check_op_ordering(struct nfsd4_compoundargs *args)
{
	struct nfsd4_op *first_op = &args->ops[0];

	/* These ordering requirements don't apply to NFSv4.0: */
	if (args->minorversion == 0)
		return nfs_ok;
	/* This is weird, but OK, not our problem: */
	if (args->opcnt == 0)
		return nfs_ok;
	if (first_op->status == nfserr_op_illegal)
		return nfs_ok;
	if (!(nfsd4_ops[first_op->opnum].op_flags & ALLOWED_AS_FIRST_OP))
		return nfserr_op_not_in_session;
	if (first_op->opnum == OP_SEQUENCE)
		return nfs_ok;
	/*
	 * So first_op is something allowed outside a session, like
	 * EXCHANGE_ID; but then it has to be the only op in the
	 * compound:
	 */
	if (args->opcnt != 1)
		return nfserr_not_only_op;
	return nfs_ok;
}