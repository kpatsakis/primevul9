static void nfs4_proc_read_rpc_prepare(struct rpc_task *task, struct nfs_read_data *data)
{
	if (nfs4_setup_sequence(NFS_SERVER(data->header->inode),
				&data->args.seq_args,
				&data->res.seq_res,
				task))
		return;
	rpc_call_start(task);
}