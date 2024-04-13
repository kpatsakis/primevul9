static int _nfs4_open_delegation_recall(struct nfs_open_context *ctx, struct nfs4_state *state, const nfs4_stateid *stateid)
{
	struct nfs4_opendata *opendata;
	int ret;

	opendata = nfs4_open_recoverdata_alloc(ctx, state);
	if (IS_ERR(opendata))
		return PTR_ERR(opendata);
	opendata->o_arg.claim = NFS4_OPEN_CLAIM_DELEGATE_CUR;
	nfs4_stateid_copy(&opendata->o_arg.u.delegation, stateid);
	ret = nfs4_open_recover(opendata, state);
	nfs4_opendata_put(opendata);
	return ret;
}