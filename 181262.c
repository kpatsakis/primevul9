static inline u32 nfsd4_setattr_rsize(struct svc_rqst *rqstp, struct nfsd4_op *op)
{
	return (op_encode_hdr_size + nfs4_fattr_bitmap_maxsz) * sizeof(__be32);
}