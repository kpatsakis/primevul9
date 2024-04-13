static inline u32 nfsd4_sequence_rsize(struct svc_rqst *rqstp,
				       struct nfsd4_op *op)
{
	return (op_encode_hdr_size
		+ XDR_QUADLEN(NFS4_MAX_SESSIONID_LEN) + 5) * sizeof(__be32);
}