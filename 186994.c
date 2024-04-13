static void nfs41_sequence_release(void *data)
{
	struct nfs4_sequence_data *calldata = data;
	struct nfs_client *clp = calldata->clp;

	if (atomic_read(&clp->cl_count) > 1)
		nfs4_schedule_state_renewal(clp);
	nfs_put_client(clp);
	kfree(calldata);
}