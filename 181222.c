static inline u32 nfsd4_layoutget_rsize(struct svc_rqst *rqstp, struct nfsd4_op *op)
{
	return (op_encode_hdr_size +
		1 /* logr_return_on_close */ +
		op_encode_stateid_maxsz +
		1 /* nr of layouts */ +
		MAX_LAYOUT_SIZE) * sizeof(__be32);
}