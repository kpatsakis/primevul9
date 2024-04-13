static inline u32 nfsd4_layoutcommit_rsize(struct svc_rqst *rqstp, struct nfsd4_op *op)
{
	return (op_encode_hdr_size +
		1 /* locr_newsize */ +
		2 /* ns_size */) * sizeof(__be32);
}