static inline u32 nfsd4_copy_rsize(struct svc_rqst *rqstp, struct nfsd4_op *op)
{
	return (op_encode_hdr_size +
		1 /* wr_callback */ +
		op_encode_stateid_maxsz /* wr_callback */ +
		2 /* wr_count */ +
		1 /* wr_committed */ +
		op_encode_verifier_maxsz +
		1 /* cr_consecutive */ +
		1 /* cr_synchronous */) * sizeof(__be32);
}