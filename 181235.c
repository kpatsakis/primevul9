int nfsd4_max_reply(struct svc_rqst *rqstp, struct nfsd4_op *op)
{
	if (op->opnum == OP_ILLEGAL || op->status == nfserr_notsupp)
		return op_encode_hdr_size * sizeof(__be32);

	BUG_ON(OPDESC(op)->op_rsize_bop == NULL);
	return OPDESC(op)->op_rsize_bop(rqstp, op);
}