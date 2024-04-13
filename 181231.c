static inline u32 nfsd4_status_stateid_rsize(struct svc_rqst *rqstp, struct nfsd4_op *op)
{
	return (op_encode_hdr_size + op_encode_stateid_maxsz)* sizeof(__be32);
}