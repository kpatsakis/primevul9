smb2_readv_callback(struct mid_q_entry *mid)
{
	struct cifs_readdata *rdata = mid->callback_data;
	struct cifs_tcon *tcon = tlink_tcon(rdata->cfile->tlink);
	struct TCP_Server_Info *server = tcon->ses->server;
	struct smb2_sync_hdr *shdr =
				(struct smb2_sync_hdr *)rdata->iov[0].iov_base;
	struct cifs_credits credits = { .value = 0, .instance = 0 };
	struct smb_rqst rqst = { .rq_iov = rdata->iov,
				 .rq_nvec = 2,
				 .rq_pages = rdata->pages,
				 .rq_offset = rdata->page_offset,
				 .rq_npages = rdata->nr_pages,
				 .rq_pagesz = rdata->pagesz,
				 .rq_tailsz = rdata->tailsz };

	cifs_dbg(FYI, "%s: mid=%llu state=%d result=%d bytes=%u\n",
		 __func__, mid->mid, mid->mid_state, rdata->result,
		 rdata->bytes);

	switch (mid->mid_state) {
	case MID_RESPONSE_RECEIVED:
		credits.value = le16_to_cpu(shdr->CreditRequest);
		credits.instance = server->reconnect_instance;
		/* result already set, check signature */
		if (server->sign && !mid->decrypted) {
			int rc;

			rc = smb2_verify_signature(&rqst, server);
			if (rc)
				cifs_dbg(VFS, "SMB signature verification returned error = %d\n",
					 rc);
		}
		/* FIXME: should this be counted toward the initiating task? */
		task_io_account_read(rdata->got_bytes);
		cifs_stats_bytes_read(tcon, rdata->got_bytes);
		break;
	case MID_REQUEST_SUBMITTED:
	case MID_RETRY_NEEDED:
		rdata->result = -EAGAIN;
		if (server->sign && rdata->got_bytes)
			/* reset bytes number since we can not check a sign */
			rdata->got_bytes = 0;
		/* FIXME: should this be counted toward the initiating task? */
		task_io_account_read(rdata->got_bytes);
		cifs_stats_bytes_read(tcon, rdata->got_bytes);
		break;
	case MID_RESPONSE_MALFORMED:
		credits.value = le16_to_cpu(shdr->CreditRequest);
		credits.instance = server->reconnect_instance;
		/* fall through */
	default:
		rdata->result = -EIO;
	}
#ifdef CONFIG_CIFS_SMB_DIRECT
	/*
	 * If this rdata has a memmory registered, the MR can be freed
	 * MR needs to be freed as soon as I/O finishes to prevent deadlock
	 * because they have limited number and are used for future I/Os
	 */
	if (rdata->mr) {
		smbd_deregister_mr(rdata->mr);
		rdata->mr = NULL;
	}
#endif
	if (rdata->result && rdata->result != -ENODATA) {
		cifs_stats_fail_inc(tcon, SMB2_READ_HE);
		trace_smb3_read_err(0 /* xid */,
				    rdata->cfile->fid.persistent_fid,
				    tcon->tid, tcon->ses->Suid, rdata->offset,
				    rdata->bytes, rdata->result);
	} else
		trace_smb3_read_done(0 /* xid */,
				     rdata->cfile->fid.persistent_fid,
				     tcon->tid, tcon->ses->Suid,
				     rdata->offset, rdata->got_bytes);

	queue_work(cifsiod_wq, &rdata->work);
	DeleteMidQEntry(mid);
	add_credits(server, &credits, 0);
}