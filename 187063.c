int nfs4_lock_delegation_recall(struct nfs4_state *state, struct file_lock *fl)
{
	struct nfs_server *server = NFS_SERVER(state->inode);
	struct nfs4_exception exception = { };
	int err;

	err = nfs4_set_lock_state(state, fl);
	if (err != 0)
		goto out;
	do {
		err = _nfs4_do_setlk(state, F_SETLK, fl, NFS_LOCK_NEW);
		switch (err) {
			default:
				printk(KERN_ERR "NFS: %s: unhandled error "
					"%d.\n", __func__, err);
			case 0:
			case -ESTALE:
				goto out;
			case -NFS4ERR_EXPIRED:
				nfs4_schedule_stateid_recovery(server, state);
			case -NFS4ERR_STALE_CLIENTID:
			case -NFS4ERR_STALE_STATEID:
				nfs4_schedule_lease_recovery(server->nfs_client);
				goto out;
			case -NFS4ERR_BADSESSION:
			case -NFS4ERR_BADSLOT:
			case -NFS4ERR_BAD_HIGH_SLOT:
			case -NFS4ERR_CONN_NOT_BOUND_TO_SESSION:
			case -NFS4ERR_DEADSESSION:
				nfs4_schedule_session_recovery(server->nfs_client->cl_session, err);
				goto out;
			case -ERESTARTSYS:
				/*
				 * The show must go on: exit, but mark the
				 * stateid as needing recovery.
				 */
			case -NFS4ERR_DELEG_REVOKED:
			case -NFS4ERR_ADMIN_REVOKED:
			case -NFS4ERR_BAD_STATEID:
			case -NFS4ERR_OPENMODE:
				nfs4_schedule_stateid_recovery(server, state);
				err = 0;
				goto out;
			case -EKEYEXPIRED:
				/*
				 * User RPCSEC_GSS context has expired.
				 * We cannot recover this stateid now, so
				 * skip it and allow recovery thread to
				 * proceed.
				 */
				err = 0;
				goto out;
			case -ENOMEM:
			case -NFS4ERR_DENIED:
				/* kill_proc(fl->fl_pid, SIGLOST, 1); */
				err = 0;
				goto out;
			case -NFS4ERR_DELAY:
				break;
		}
		err = nfs4_handle_exception(server, err, &exception);
	} while (exception.retry);
out:
	return err;
}