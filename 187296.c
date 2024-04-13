static void nfs4_proc_commit_setup(struct nfs_commit_data *data, struct rpc_message *msg)
{
	struct nfs_server *server = NFS_SERVER(data->inode);

	if (data->commit_done_cb == NULL)
		data->commit_done_cb = nfs4_commit_done_cb;
	data->res.server = server;
	msg->rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_COMMIT];
	nfs41_init_sequence(&data->args.seq_args, &data->res.seq_res, 1);
}