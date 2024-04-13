int nfs4_call_sync(struct rpc_clnt *clnt,
		   struct nfs_server *server,
		   struct rpc_message *msg,
		   struct nfs4_sequence_args *args,
		   struct nfs4_sequence_res *res,
		   int cache_reply)
{
	return server->nfs_client->cl_mvops->call_sync(clnt, server, msg,
						args, res, cache_reply);
}