static int _nfs4_free_stateid(struct nfs_server *server, nfs4_stateid *stateid)
{
	struct nfs41_free_stateid_args args = {
		.stateid = stateid,
	};
	struct nfs41_free_stateid_res res;
	struct rpc_message msg = {
		.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_FREE_STATEID],
		.rpc_argp = &args,
		.rpc_resp = &res,
	};
	int status;

	dprintk("NFS call  free_stateid %p\n", stateid);
	nfs41_init_sequence(&args.seq_args, &res.seq_res, 0);
	status = nfs4_call_sync_sequence(server->client, server, &msg,
					 &args.seq_args, &res.seq_res, 1);
	dprintk("NFS reply free_stateid: %d\n", status);
	return status;
}