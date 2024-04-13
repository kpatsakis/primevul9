static int nfsd4_cb_offload_done(struct nfsd4_callback *cb,
				 struct rpc_task *task)
{
	return 1;
}