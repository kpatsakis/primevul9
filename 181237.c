static inline u32 nfsd4_readdir_rsize(struct svc_rqst *rqstp, struct nfsd4_op *op)
{
	u32 maxcount = 0, rlen = 0;

	maxcount = svc_max_payload(rqstp);
	rlen = min(op->u.readdir.rd_maxcount, maxcount);

	return (op_encode_hdr_size + op_encode_verifier_maxsz +
		XDR_QUADLEN(rlen)) * sizeof(__be32);
}