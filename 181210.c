static inline u32 nfsd4_getfh_rsize(struct svc_rqst *rqstp, struct nfsd4_op *op)
{
	return (op_encode_hdr_size + 1) * sizeof(__be32) + NFS4_FHSIZE;
}