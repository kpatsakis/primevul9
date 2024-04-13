static inline u32 nfsd4_secinfo_rsize(struct svc_rqst *rqstp, struct nfsd4_op *op)
{
	return (op_encode_hdr_size + RPC_AUTH_MAXFLAVOR *
		(4 + XDR_QUADLEN(GSS_OID_MAX_LEN))) * sizeof(__be32);
}