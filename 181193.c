static inline u32 nfsd4_access_rsize(struct svc_rqst *rqstp, struct nfsd4_op *op)
{
	/* ac_supported, ac_resp_access */
	return (op_encode_hdr_size + 2)* sizeof(__be32);
}