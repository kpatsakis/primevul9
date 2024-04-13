static inline u32 nfsd4_lock_rsize(struct svc_rqst *rqstp, struct nfsd4_op *op)
{
	return (op_encode_hdr_size + op_encode_lock_denied_maxsz)
		* sizeof(__be32);
}