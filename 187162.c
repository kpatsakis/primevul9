int nfs4_destroy_clientid(struct nfs_client *clp)
{
	struct rpc_cred *cred;
	int ret = 0;

	if (clp->cl_mvops->minor_version < 1)
		goto out;
	if (clp->cl_exchange_flags == 0)
		goto out;
	cred = nfs4_get_exchange_id_cred(clp);
	ret = nfs4_proc_destroy_clientid(clp, cred);
	if (cred)
		put_rpccred(cred);
	switch (ret) {
	case 0:
	case -NFS4ERR_STALE_CLIENTID:
		clp->cl_exchange_flags = 0;
	}
out:
	return ret;
}