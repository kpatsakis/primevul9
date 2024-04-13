static inline u32 nfsd4_read_rsize(struct svc_rqst *rqstp, struct nfsd4_op *op)
{
	u32 maxcount = 0, rlen = 0;

	maxcount = svc_max_payload(rqstp);
	rlen = min(op->u.read.rd_length, maxcount);

	return (op_encode_hdr_size + 2 + XDR_QUADLEN(rlen)) * sizeof(__be32);
}