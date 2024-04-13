static inline u32 nfsd4_layoutreturn_rsize(struct svc_rqst *rqstp, struct nfsd4_op *op)
{
	return (op_encode_hdr_size +
		1 /* lrs_stateid */ +
		op_encode_stateid_maxsz) * sizeof(__be32);
}