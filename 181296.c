static inline u32 nfsd4_offload_status_rsize(struct svc_rqst *rqstp,
					     struct nfsd4_op *op)
{
	return (op_encode_hdr_size +
		2 /* osr_count */ +
		1 /* osr_complete<1> optional 0 for now */) * sizeof(__be32);
}