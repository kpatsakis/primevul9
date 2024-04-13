static void nfs4_delegreturn_prepare(struct rpc_task *task, void *data)
{
	struct nfs4_delegreturndata *d_data;

	d_data = (struct nfs4_delegreturndata *)data;

	if (nfs4_setup_sequence(d_data->res.server,
				&d_data->args.seq_args,
				&d_data->res.seq_res, task))
		return;
	rpc_call_start(task);
}