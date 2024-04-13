void smb2_reconnect_server(struct work_struct *work)
{
	struct TCP_Server_Info *server = container_of(work,
					struct TCP_Server_Info, reconnect.work);
	struct cifs_ses *ses;
	struct cifs_tcon *tcon, *tcon2;
	struct list_head tmp_list;
	int tcon_exist = false;
	int rc;
	int resched = false;


	/* Prevent simultaneous reconnects that can corrupt tcon->rlist list */
	mutex_lock(&server->reconnect_mutex);

	INIT_LIST_HEAD(&tmp_list);
	cifs_dbg(FYI, "Need negotiate, reconnecting tcons\n");

	spin_lock(&cifs_tcp_ses_lock);
	list_for_each_entry(ses, &server->smb_ses_list, smb_ses_list) {
		list_for_each_entry(tcon, &ses->tcon_list, tcon_list) {
			if (tcon->need_reconnect || tcon->need_reopen_files) {
				tcon->tc_count++;
				list_add_tail(&tcon->rlist, &tmp_list);
				tcon_exist = true;
			}
		}
		if (ses->tcon_ipc && ses->tcon_ipc->need_reconnect) {
			list_add_tail(&ses->tcon_ipc->rlist, &tmp_list);
			tcon_exist = true;
		}
	}
	/*
	 * Get the reference to server struct to be sure that the last call of
	 * cifs_put_tcon() in the loop below won't release the server pointer.
	 */
	if (tcon_exist)
		server->srv_count++;

	spin_unlock(&cifs_tcp_ses_lock);

	list_for_each_entry_safe(tcon, tcon2, &tmp_list, rlist) {
		rc = smb2_reconnect(SMB2_INTERNAL_CMD, tcon);
		if (!rc)
			cifs_reopen_persistent_handles(tcon);
		else
			resched = true;
		list_del_init(&tcon->rlist);
		cifs_put_tcon(tcon);
	}

	cifs_dbg(FYI, "Reconnecting tcons finished\n");
	if (resched)
		queue_delayed_work(cifsiod_wq, &server->reconnect, 2 * HZ);
	mutex_unlock(&server->reconnect_mutex);

	/* now we can safely release srv struct */
	if (tcon_exist)
		cifs_put_tcp_session(server, 1);
}