static inline u32 nfsd4_test_stateid_rsize(struct svc_rqst *rqstp, struct nfsd4_op *op)
{
	return (op_encode_hdr_size + 1 + op->u.test_stateid.ts_num_ids)
		* sizeof(__be32);
}