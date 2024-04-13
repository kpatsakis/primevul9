static inline u32 nfsd4_only_status_rsize(struct svc_rqst *rqstp, struct nfsd4_op *op)
{
	return (op_encode_hdr_size) * sizeof(__be32);
}