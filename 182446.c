smb2_reconnect(__le16 smb2_command, struct cifs_tcon *tcon)
{
	int rc;
	struct nls_table *nls_codepage;
	struct cifs_ses *ses;
	struct TCP_Server_Info *server;
	int retries;

	/*
	 * SMB2s NegProt, SessSetup, Logoff do not have tcon yet so
	 * check for tcp and smb session status done differently
	 * for those three - in the calling routine.
	 */
	if (tcon == NULL)
		return 0;

	if (smb2_command == SMB2_TREE_CONNECT)
		return 0;

	if (tcon->tidStatus == CifsExiting) {
		/*
		 * only tree disconnect, open, and write,
		 * (and ulogoff which does not have tcon)
		 * are allowed as we start force umount.
		 */
		if ((smb2_command != SMB2_WRITE) &&
		   (smb2_command != SMB2_CREATE) &&
		   (smb2_command != SMB2_TREE_DISCONNECT)) {
			cifs_dbg(FYI, "can not send cmd %d while umounting\n",
				 smb2_command);
			return -ENODEV;
		}
	}
	if ((!tcon->ses) || (tcon->ses->status == CifsExiting) ||
	    (!tcon->ses->server))
		return -EIO;

	ses = tcon->ses;
	server = ses->server;

	retries = server->nr_targets;

	/*
	 * Give demultiplex thread up to 10 seconds to each target available for
	 * reconnect -- should be greater than cifs socket timeout which is 7
	 * seconds.
	 */
	while (server->tcpStatus == CifsNeedReconnect) {
		/*
		 * Return to caller for TREE_DISCONNECT and LOGOFF and CLOSE
		 * here since they are implicitly done when session drops.
		 */
		switch (smb2_command) {
		/*
		 * BB Should we keep oplock break and add flush to exceptions?
		 */
		case SMB2_TREE_DISCONNECT:
		case SMB2_CANCEL:
		case SMB2_CLOSE:
		case SMB2_OPLOCK_BREAK:
			return -EAGAIN;
		}

		rc = wait_event_interruptible_timeout(server->response_q,
						      (server->tcpStatus != CifsNeedReconnect),
						      10 * HZ);
		if (rc < 0) {
			cifs_dbg(FYI, "%s: aborting reconnect due to a received"
				 " signal by the process\n", __func__);
			return -ERESTARTSYS;
		}

		/* are we still trying to reconnect? */
		if (server->tcpStatus != CifsNeedReconnect)
			break;

		if (--retries)
			continue;

		/*
		 * on "soft" mounts we wait once. Hard mounts keep
		 * retrying until process is killed or server comes
		 * back on-line
		 */
		if (!tcon->retry) {
			cifs_dbg(FYI, "gave up waiting on reconnect in smb_init\n");
			return -EHOSTDOWN;
		}
		retries = server->nr_targets;
	}

	if (!tcon->ses->need_reconnect && !tcon->need_reconnect)
		return 0;

	nls_codepage = load_nls_default();

	/*
	 * need to prevent multiple threads trying to simultaneously reconnect
	 * the same SMB session
	 */
	mutex_lock(&tcon->ses->session_mutex);

	/*
	 * Recheck after acquire mutex. If another thread is negotiating
	 * and the server never sends an answer the socket will be closed
	 * and tcpStatus set to reconnect.
	 */
	if (server->tcpStatus == CifsNeedReconnect) {
		rc = -EHOSTDOWN;
		mutex_unlock(&tcon->ses->session_mutex);
		goto out;
	}

	rc = cifs_negotiate_protocol(0, tcon->ses);
	if (!rc && tcon->ses->need_reconnect)
		rc = cifs_setup_session(0, tcon->ses, nls_codepage);

	if (rc || !tcon->need_reconnect) {
		mutex_unlock(&tcon->ses->session_mutex);
		goto out;
	}

	cifs_mark_open_files_invalid(tcon);
	if (tcon->use_persistent)
		tcon->need_reopen_files = true;

	rc = __smb2_reconnect(nls_codepage, tcon);
	mutex_unlock(&tcon->ses->session_mutex);

	cifs_dbg(FYI, "reconnect tcon rc = %d\n", rc);
	if (rc) {
		/* If sess reconnected but tcon didn't, something strange ... */
		printk_once(KERN_WARNING "reconnect tcon failed rc = %d\n", rc);
		goto out;
	}

	if (smb2_command != SMB2_INTERNAL_CMD)
		queue_delayed_work(cifsiod_wq, &server->reconnect, 0);

	atomic_inc(&tconInfoReconnectCount);
out:
	/*
	 * Check if handle based operation so we know whether we can continue
	 * or not without returning to caller to reset file handle.
	 */
	/*
	 * BB Is flush done by server on drop of tcp session? Should we special
	 * case it and skip above?
	 */
	switch (smb2_command) {
	case SMB2_FLUSH:
	case SMB2_READ:
	case SMB2_WRITE:
	case SMB2_LOCK:
	case SMB2_IOCTL:
	case SMB2_QUERY_DIRECTORY:
	case SMB2_CHANGE_NOTIFY:
	case SMB2_QUERY_INFO:
	case SMB2_SET_INFO:
		rc = -EAGAIN;
	}
	unload_nls(nls_codepage);
	return rc;
}