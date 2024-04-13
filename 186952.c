static int nfs4_client_recover_expired_lease(struct nfs_client *clp)
{
	unsigned int loop;
	int ret;

	for (loop = NFS4_MAX_LOOP_ON_RECOVER; loop != 0; loop--) {
		ret = nfs4_wait_clnt_recover(clp);
		if (ret != 0)
			break;
		if (!test_bit(NFS4CLNT_LEASE_EXPIRED, &clp->cl_state) &&
		    !test_bit(NFS4CLNT_CHECK_LEASE,&clp->cl_state))
			break;
		nfs4_schedule_state_manager(clp);
		ret = -EIO;
	}
	return ret;
}