static int nfs4_wait_clnt_recover(struct nfs_client *clp)
{
	int res;

	might_sleep();

	res = wait_on_bit(&clp->cl_state, NFS4CLNT_MANAGER_RUNNING,
			nfs_wait_bit_killable, TASK_KILLABLE);
	if (res)
		return res;

	if (clp->cl_cons_state < 0)
		return clp->cl_cons_state;
	return 0;
}