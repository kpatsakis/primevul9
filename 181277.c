static inline u32 nfsd4_seek_rsize(struct svc_rqst *rqstp, struct nfsd4_op *op)
{
	return (op_encode_hdr_size + 3) * sizeof(__be32);
}