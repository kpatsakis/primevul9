static void nfs4_proc_read_setup(struct nfs_read_data *data, struct rpc_message *msg)
{
	data->timestamp   = jiffies;
	data->read_done_cb = nfs4_read_done_cb;
	msg->rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_READ];
	nfs41_init_sequence(&data->args.seq_args, &data->res.seq_res, 0);
}