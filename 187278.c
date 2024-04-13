int nfs4_init_session(struct nfs_server *server)
{
	struct nfs_client *clp = server->nfs_client;
	struct nfs4_session *session;
	unsigned int rsize, wsize;

	if (!nfs4_has_session(clp))
		return 0;

	session = clp->cl_session;
	spin_lock(&clp->cl_lock);
	if (test_and_clear_bit(NFS4_SESSION_INITING, &session->session_state)) {

		rsize = server->rsize;
		if (rsize == 0)
			rsize = NFS_MAX_FILE_IO_SIZE;
		wsize = server->wsize;
		if (wsize == 0)
			wsize = NFS_MAX_FILE_IO_SIZE;

		session->fc_attrs.max_rqst_sz = wsize + nfs41_maxwrite_overhead;
		session->fc_attrs.max_resp_sz = rsize + nfs41_maxread_overhead;
	}
	spin_unlock(&clp->cl_lock);

	return nfs41_check_session_ready(clp);
}