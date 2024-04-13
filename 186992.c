int _nfs4_call_sync(struct rpc_clnt *clnt,
		    struct nfs_server *server,
		    struct rpc_message *msg,
		    struct nfs4_sequence_args *args,
		    struct nfs4_sequence_res *res,
		    int cache_reply)
{
	nfs41_init_sequence(args, res, cache_reply);
	return rpc_call_sync(clnt, msg, 0);
}