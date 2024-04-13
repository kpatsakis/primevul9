static inline u32 nfsd4_getdeviceinfo_rsize(struct svc_rqst *rqstp, struct nfsd4_op *op)
{
	u32 maxcount = 0, rlen = 0;

	maxcount = svc_max_payload(rqstp);
	rlen = min(op->u.getdeviceinfo.gd_maxcount, maxcount);

	return (op_encode_hdr_size +
		1 /* gd_layout_type*/ +
		XDR_QUADLEN(rlen) +
		2 /* gd_notify_types */) * sizeof(__be32);
}