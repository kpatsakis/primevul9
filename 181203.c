static inline u32 nfsd4_setclientid_rsize(struct svc_rqst *rqstp, struct nfsd4_op *op)
{
	return (op_encode_hdr_size + 2 + XDR_QUADLEN(NFS4_VERIFIER_SIZE)) *
								sizeof(__be32);
}