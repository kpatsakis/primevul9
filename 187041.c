int nfs4_proc_create_session(struct nfs_client *clp, struct rpc_cred *cred)
{
	int status;
	unsigned *ptr;
	struct nfs4_session *session = clp->cl_session;

	dprintk("--> %s clp=%p session=%p\n", __func__, clp, session);

	status = _nfs4_proc_create_session(clp, cred);
	if (status)
		goto out;

	/* Init or reset the session slot tables */
	status = nfs4_setup_session_slot_tables(session);
	dprintk("slot table setup returned %d\n", status);
	if (status)
		goto out;

	ptr = (unsigned *)&session->sess_id.data[0];
	dprintk("%s client>seqid %d sessionid %u:%u:%u:%u\n", __func__,
		clp->cl_seqid, ptr[0], ptr[1], ptr[2], ptr[3]);
out:
	dprintk("<-- %s\n", __func__);
	return status;
}