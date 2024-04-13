static inline u32 nfsd4_write_rsize(struct svc_rqst *rqstp, struct nfsd4_op *op)
{
	return (op_encode_hdr_size + 2 + op_encode_verifier_maxsz) * sizeof(__be32);
}