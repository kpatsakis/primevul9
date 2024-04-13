static int nfs4_handle_exception(struct nfs_server *server, int errorcode, struct nfs4_exception *exception)
{
	struct nfs_client *clp = server->nfs_client;
	struct nfs4_state *state = exception->state;
	struct inode *inode = exception->inode;
	int ret = errorcode;

	exception->retry = 0;
	switch(errorcode) {
		case 0:
			return 0;
		case -NFS4ERR_OPENMODE:
			if (inode && nfs4_have_delegation(inode, FMODE_READ)) {
				nfs4_inode_return_delegation(inode);
				exception->retry = 1;
				return 0;
			}
			if (state == NULL)
				break;
			nfs4_schedule_stateid_recovery(server, state);
			goto wait_on_recovery;
		case -NFS4ERR_DELEG_REVOKED:
		case -NFS4ERR_ADMIN_REVOKED:
		case -NFS4ERR_BAD_STATEID:
			if (state == NULL)
				break;
			nfs_remove_bad_delegation(state->inode);
			nfs4_schedule_stateid_recovery(server, state);
			goto wait_on_recovery;
		case -NFS4ERR_EXPIRED:
			if (state != NULL)
				nfs4_schedule_stateid_recovery(server, state);
		case -NFS4ERR_STALE_STATEID:
		case -NFS4ERR_STALE_CLIENTID:
			nfs4_schedule_lease_recovery(clp);
			goto wait_on_recovery;
#if defined(CONFIG_NFS_V4_1)
		case -NFS4ERR_BADSESSION:
		case -NFS4ERR_BADSLOT:
		case -NFS4ERR_BAD_HIGH_SLOT:
		case -NFS4ERR_CONN_NOT_BOUND_TO_SESSION:
		case -NFS4ERR_DEADSESSION:
		case -NFS4ERR_SEQ_FALSE_RETRY:
		case -NFS4ERR_SEQ_MISORDERED:
			dprintk("%s ERROR: %d Reset session\n", __func__,
				errorcode);
			nfs4_schedule_session_recovery(clp->cl_session, errorcode);
			exception->retry = 1;
			break;
#endif /* defined(CONFIG_NFS_V4_1) */
		case -NFS4ERR_FILE_OPEN:
			if (exception->timeout > HZ) {
				/* We have retried a decent amount, time to
				 * fail
				 */
				ret = -EBUSY;
				break;
			}
		case -NFS4ERR_GRACE:
		case -NFS4ERR_DELAY:
		case -EKEYEXPIRED:
			ret = nfs4_delay(server->client, &exception->timeout);
			if (ret != 0)
				break;
		case -NFS4ERR_RETRY_UNCACHED_REP:
		case -NFS4ERR_OLD_STATEID:
			exception->retry = 1;
			break;
		case -NFS4ERR_BADOWNER:
			/* The following works around a Linux server bug! */
		case -NFS4ERR_BADNAME:
			if (server->caps & NFS_CAP_UIDGID_NOMAP) {
				server->caps &= ~NFS_CAP_UIDGID_NOMAP;
				exception->retry = 1;
				printk(KERN_WARNING "NFS: v4 server %s "
						"does not accept raw "
						"uid/gids. "
						"Reenabling the idmapper.\n",
						server->nfs_client->cl_hostname);
			}
	}
	/* We failed to handle the error */
	return nfs4_map_errors(ret);
wait_on_recovery:
	ret = nfs4_wait_clnt_recover(clp);
	if (ret == 0)
		exception->retry = 1;
	return ret;
}