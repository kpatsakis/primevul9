int nfs4_release_lockowner(struct nfs4_lock_state *lsp)
{
	struct nfs_server *server = lsp->ls_state->owner->so_server;
	struct nfs_release_lockowner_data *data;
	struct rpc_message msg = {
		.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_RELEASE_LOCKOWNER],
	};

	if (server->nfs_client->cl_mvops->minor_version != 0)
		return -EINVAL;
	data = kmalloc(sizeof(*data), GFP_NOFS);
	if (!data)
		return -ENOMEM;
	data->lsp = lsp;
	data->server = server;
	data->args.lock_owner.clientid = server->nfs_client->cl_clientid;
	data->args.lock_owner.id = lsp->ls_seqid.owner_id;
	data->args.lock_owner.s_dev = server->s_dev;
	msg.rpc_argp = &data->args;
	rpc_call_async(server->client, &msg, 0, &nfs4_release_lockowner_ops, data);
	return 0;
}