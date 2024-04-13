nfs4_layoutreturn_prepare(struct rpc_task *task, void *calldata)
{
	struct nfs4_layoutreturn *lrp = calldata;

	dprintk("--> %s\n", __func__);
	if (nfs41_setup_sequence(lrp->clp->cl_session, &lrp->args.seq_args,
				&lrp->res.seq_res, task))
		return;
	rpc_call_start(task);
}