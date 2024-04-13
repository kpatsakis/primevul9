int nfs4_open_delegation_recall(struct nfs_open_context *ctx, struct nfs4_state *state, const nfs4_stateid *stateid)
{
	struct nfs4_exception exception = { };
	struct nfs_server *server = NFS_SERVER(state->inode);
	int err;
	do {
		err = _nfs4_open_delegation_recall(ctx, state, stateid);
		switch (err) {
			case 0:
			case -ENOENT:
			case -ESTALE:
				goto out;
			case -NFS4ERR_BADSESSION:
			case -NFS4ERR_BADSLOT:
			case -NFS4ERR_BAD_HIGH_SLOT:
			case -NFS4ERR_CONN_NOT_BOUND_TO_SESSION:
			case -NFS4ERR_DEADSESSION:
				nfs4_schedule_session_recovery(server->nfs_client->cl_session, err);
				goto out;
			case -NFS4ERR_STALE_CLIENTID:
			case -NFS4ERR_STALE_STATEID:
			case -NFS4ERR_EXPIRED:
				/* Don't recall a delegation if it was lost */
				nfs4_schedule_lease_recovery(server->nfs_client);
				goto out;
			case -ERESTARTSYS:
				/*
				 * The show must go on: exit, but mark the
				 * stateid as needing recovery.
				 */
			case -NFS4ERR_DELEG_REVOKED:
			case -NFS4ERR_ADMIN_REVOKED:
			case -NFS4ERR_BAD_STATEID:
				nfs_inode_find_state_and_recover(state->inode,
						stateid);
				nfs4_schedule_stateid_recovery(server, state);
			case -EKEYEXPIRED:
				/*
				 * User RPCSEC_GSS context has expired.
				 * We cannot recover this stateid now, so
				 * skip it and allow recovery thread to
				 * proceed.
				 */
			case -ENOMEM:
				err = 0;
				goto out;
		}
		err = nfs4_handle_exception(server, err, &exception);
	} while (exception.retry);
out:
	return err;
}