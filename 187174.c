static struct nfs4_opendata *nfs4_open_recoverdata_alloc(struct nfs_open_context *ctx, struct nfs4_state *state)
{
	struct nfs4_opendata *opendata;

	opendata = nfs4_opendata_alloc(ctx->dentry, state->owner, 0, 0, NULL, GFP_NOFS);
	if (opendata == NULL)
		return ERR_PTR(-ENOMEM);
	opendata->state = state;
	atomic_inc(&state->count);
	return opendata;
}