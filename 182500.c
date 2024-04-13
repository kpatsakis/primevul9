smb2_async_readv(struct cifs_readdata *rdata)
{
	int rc, flags = 0;
	char *buf;
	struct smb2_sync_hdr *shdr;
	struct cifs_io_parms io_parms;
	struct smb_rqst rqst = { .rq_iov = rdata->iov,
				 .rq_nvec = 1 };
	struct TCP_Server_Info *server;
	unsigned int total_len;

	cifs_dbg(FYI, "%s: offset=%llu bytes=%u\n",
		 __func__, rdata->offset, rdata->bytes);

	io_parms.tcon = tlink_tcon(rdata->cfile->tlink);
	io_parms.offset = rdata->offset;
	io_parms.length = rdata->bytes;
	io_parms.persistent_fid = rdata->cfile->fid.persistent_fid;
	io_parms.volatile_fid = rdata->cfile->fid.volatile_fid;
	io_parms.pid = rdata->pid;

	server = io_parms.tcon->ses->server;

	rc = smb2_new_read_req(
		(void **) &buf, &total_len, &io_parms, rdata, 0, 0);
	if (rc)
		return rc;

	if (smb3_encryption_required(io_parms.tcon))
		flags |= CIFS_TRANSFORM_REQ;

	rdata->iov[0].iov_base = buf;
	rdata->iov[0].iov_len = total_len;

	shdr = (struct smb2_sync_hdr *)buf;

	if (rdata->credits.value > 0) {
		shdr->CreditCharge = cpu_to_le16(DIV_ROUND_UP(rdata->bytes,
						SMB2_MAX_BUFFER_SIZE));
		shdr->CreditRequest =
			cpu_to_le16(le16_to_cpu(shdr->CreditCharge) + 1);

		rc = adjust_credits(server, &rdata->credits, rdata->bytes);
		if (rc)
			goto async_readv_out;

		flags |= CIFS_HAS_CREDITS;
	}

	kref_get(&rdata->refcount);
	rc = cifs_call_async(io_parms.tcon->ses->server, &rqst,
			     cifs_readv_receive, smb2_readv_callback,
			     smb3_handle_read_data, rdata, flags,
			     &rdata->credits);
	if (rc) {
		kref_put(&rdata->refcount, cifs_readdata_release);
		cifs_stats_fail_inc(io_parms.tcon, SMB2_READ_HE);
		trace_smb3_read_err(0 /* xid */, io_parms.persistent_fid,
				    io_parms.tcon->tid,
				    io_parms.tcon->ses->Suid,
				    io_parms.offset, io_parms.length, rc);
	}

async_readv_out:
	cifs_small_buf_release(buf);
	return rc;
}