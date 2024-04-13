static inline u32 nfsd4_bind_conn_to_session_rsize(struct svc_rqst *rqstp, struct nfsd4_op *op)
{
	return (op_encode_hdr_size + \
		XDR_QUADLEN(NFS4_MAX_SESSIONID_LEN) + /* bctsr_sessid */\
		2 /* bctsr_dir, use_conn_in_rdma_mode */) * sizeof(__be32);
}