static inline u32 nfsd4_exchange_id_rsize(struct svc_rqst *rqstp, struct nfsd4_op *op)
{
	return (op_encode_hdr_size + 2 + 1 + /* eir_clientid, eir_sequenceid */\
		1 + 1 + /* eir_flags, spr_how */\
		4 + /* spo_must_enforce & _allow with bitmap */\
		2 + /*eir_server_owner.so_minor_id */\
		/* eir_server_owner.so_major_id<> */\
		XDR_QUADLEN(NFS4_OPAQUE_LIMIT) + 1 +\
		/* eir_server_scope<> */\
		XDR_QUADLEN(NFS4_OPAQUE_LIMIT) + 1 +\
		1 + /* eir_server_impl_id array length */\
		0 /* ignored eir_server_impl_id contents */) * sizeof(__be32);
}