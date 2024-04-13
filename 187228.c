static int nfs4_proc_async_renew(struct nfs_client *clp, struct rpc_cred *cred, unsigned renew_flags)
{
	struct rpc_message msg = {
		.rpc_proc	= &nfs4_procedures[NFSPROC4_CLNT_RENEW],
		.rpc_argp	= clp,
		.rpc_cred	= cred,
	};
	struct nfs4_renewdata *data;

	if (renew_flags == 0)
		return 0;
	if (!atomic_inc_not_zero(&clp->cl_count))
		return -EIO;
	data = kmalloc(sizeof(*data), GFP_NOFS);
	if (data == NULL)
		return -ENOMEM;
	data->client = clp;
	data->timestamp = jiffies;
	return rpc_call_async(clp->cl_rpcclient, &msg, RPC_TASK_SOFT,
			&nfs4_renew_ops, data);
}