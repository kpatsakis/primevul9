static inline u32 nfsd4_open_rsize(struct svc_rqst *rqstp, struct nfsd4_op *op)
{
	return (op_encode_hdr_size + op_encode_stateid_maxsz
		+ op_encode_change_info_maxsz + 1
		+ nfs4_fattr_bitmap_maxsz
		+ op_encode_delegation_maxsz) * sizeof(__be32);
}