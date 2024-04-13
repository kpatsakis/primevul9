int nfs4_init_ds_session(struct nfs_client *clp, unsigned long lease_time)
{
	struct nfs4_session *session = clp->cl_session;
	int ret;

	spin_lock(&clp->cl_lock);
	if (test_and_clear_bit(NFS4_SESSION_INITING, &session->session_state)) {
		/*
		 * Do not set NFS_CS_CHECK_LEASE_TIME instead set the
		 * DS lease to be equal to the MDS lease.
		 */
		clp->cl_lease_time = lease_time;
		clp->cl_last_renewal = jiffies;
	}
	spin_unlock(&clp->cl_lock);

	ret = nfs41_check_session_ready(clp);
	if (ret)
		return ret;
	/* Test for the DS role */
	if (!is_ds_client(clp))
		return -ENODEV;
	return 0;
}