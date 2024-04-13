static void encode_open_stateid(struct xdr_stream *xdr,
		const struct nfs_open_context *ctx,
		const struct nfs_lock_context *l_ctx,
		fmode_t fmode,
		int zero_seqid)
{
	nfs4_stateid stateid;

	if (ctx->state != NULL) {
		nfs4_select_rw_stateid(&stateid, ctx->state,
				fmode, l_ctx->lockowner, l_ctx->pid);
		if (zero_seqid)
			stateid.seqid = 0;
		encode_nfs4_stateid(xdr, &stateid);
	} else
		encode_nfs4_stateid(xdr, &zero_stateid);
}