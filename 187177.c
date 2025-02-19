static void nfs4_proc_write_setup(struct nfs_write_data *data, struct rpc_message *msg)
{
	struct nfs_server *server = NFS_SERVER(data->header->inode);

	if (!nfs4_write_need_cache_consistency_data(data)) {
		data->args.bitmask = NULL;
		data->res.fattr = NULL;
	} else
		data->args.bitmask = server->cache_consistency_bitmask;

	if (!data->write_done_cb)
		data->write_done_cb = nfs4_write_done_cb;
	data->res.server = server;
	data->timestamp   = jiffies;

	msg->rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_WRITE];
	nfs41_init_sequence(&data->args.seq_args, &data->res.seq_res, 1);
}