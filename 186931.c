static int nfs41_check_session_ready(struct nfs_client *clp)
{
	int ret;
	
	if (clp->cl_cons_state == NFS_CS_SESSION_INITING) {
		ret = nfs4_client_recover_expired_lease(clp);
		if (ret)
			return ret;
	}
	if (clp->cl_cons_state < NFS_CS_READY)
		return -EPROTONOSUPPORT;
	smp_rmb();
	return 0;
}