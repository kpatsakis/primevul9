int _nfs4_call_sync_session(struct rpc_clnt *clnt,
			    struct nfs_server *server,
			    struct rpc_message *msg,
			    struct nfs4_sequence_args *args,
			    struct nfs4_sequence_res *res,
			    int cache_reply)
{
	nfs41_init_sequence(args, res, cache_reply);
	return nfs4_call_sync_sequence(clnt, server, msg, args, res, 0);
}