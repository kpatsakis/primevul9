smb2_async_writev(struct cifs_writedata *wdata,
		  void (*release)(struct kref *kref))
{
	int rc = -EACCES, flags = 0;
	struct smb2_write_req *req = NULL;
	struct smb2_sync_hdr *shdr;
	struct cifs_tcon *tcon = tlink_tcon(wdata->cfile->tlink);
	struct TCP_Server_Info *server = tcon->ses->server;
	struct kvec iov[1];
	struct smb_rqst rqst = { };
	unsigned int total_len;

	rc = smb2_plain_req_init(SMB2_WRITE, tcon, (void **) &req, &total_len);
	if (rc)
		return rc;

	if (smb3_encryption_required(tcon))
		flags |= CIFS_TRANSFORM_REQ;

	shdr = (struct smb2_sync_hdr *)req;
	shdr->ProcessId = cpu_to_le32(wdata->cfile->pid);

	req->PersistentFileId = wdata->cfile->fid.persistent_fid;
	req->VolatileFileId = wdata->cfile->fid.volatile_fid;
	req->WriteChannelInfoOffset = 0;
	req->WriteChannelInfoLength = 0;
	req->Channel = 0;
	req->Offset = cpu_to_le64(wdata->offset);
	req->DataOffset = cpu_to_le16(
				offsetof(struct smb2_write_req, Buffer));
	req->RemainingBytes = 0;

	trace_smb3_write_enter(0 /* xid */, wdata->cfile->fid.persistent_fid,
		tcon->tid, tcon->ses->Suid, wdata->offset, wdata->bytes);
#ifdef CONFIG_CIFS_SMB_DIRECT
	/*
	 * If we want to do a server RDMA read, fill in and append
	 * smbd_buffer_descriptor_v1 to the end of write request
	 */
	if (server->rdma && !server->sign && wdata->bytes >=
		server->smbd_conn->rdma_readwrite_threshold) {

		struct smbd_buffer_descriptor_v1 *v1;
		bool need_invalidate = server->dialect == SMB30_PROT_ID;

		wdata->mr = smbd_register_mr(
				server->smbd_conn, wdata->pages,
				wdata->nr_pages, wdata->page_offset,
				wdata->tailsz, false, need_invalidate);
		if (!wdata->mr) {
			rc = -ENOBUFS;
			goto async_writev_out;
		}
		req->Length = 0;
		req->DataOffset = 0;
		if (wdata->nr_pages > 1)
			req->RemainingBytes =
				cpu_to_le32(
					(wdata->nr_pages - 1) * wdata->pagesz -
					wdata->page_offset + wdata->tailsz
				);
		else
			req->RemainingBytes = cpu_to_le32(wdata->tailsz);
		req->Channel = SMB2_CHANNEL_RDMA_V1_INVALIDATE;
		if (need_invalidate)
			req->Channel = SMB2_CHANNEL_RDMA_V1;
		req->WriteChannelInfoOffset =
			cpu_to_le16(offsetof(struct smb2_write_req, Buffer));
		req->WriteChannelInfoLength =
			cpu_to_le16(sizeof(struct smbd_buffer_descriptor_v1));
		v1 = (struct smbd_buffer_descriptor_v1 *) &req->Buffer[0];
		v1->offset = cpu_to_le64(wdata->mr->mr->iova);
		v1->token = cpu_to_le32(wdata->mr->mr->rkey);
		v1->length = cpu_to_le32(wdata->mr->mr->length);
	}
#endif
	iov[0].iov_len = total_len - 1;
	iov[0].iov_base = (char *)req;

	rqst.rq_iov = iov;
	rqst.rq_nvec = 1;
	rqst.rq_pages = wdata->pages;
	rqst.rq_offset = wdata->page_offset;
	rqst.rq_npages = wdata->nr_pages;
	rqst.rq_pagesz = wdata->pagesz;
	rqst.rq_tailsz = wdata->tailsz;
#ifdef CONFIG_CIFS_SMB_DIRECT
	if (wdata->mr) {
		iov[0].iov_len += sizeof(struct smbd_buffer_descriptor_v1);
		rqst.rq_npages = 0;
	}
#endif
	cifs_dbg(FYI, "async write at %llu %u bytes\n",
		 wdata->offset, wdata->bytes);

#ifdef CONFIG_CIFS_SMB_DIRECT
	/* For RDMA read, I/O size is in RemainingBytes not in Length */
	if (!wdata->mr)
		req->Length = cpu_to_le32(wdata->bytes);
#else
	req->Length = cpu_to_le32(wdata->bytes);
#endif

	if (wdata->credits.value > 0) {
		shdr->CreditCharge = cpu_to_le16(DIV_ROUND_UP(wdata->bytes,
						    SMB2_MAX_BUFFER_SIZE));
		shdr->CreditRequest =
			cpu_to_le16(le16_to_cpu(shdr->CreditCharge) + 1);

		rc = adjust_credits(server, &wdata->credits, wdata->bytes);
		if (rc)
			goto async_writev_out;

		flags |= CIFS_HAS_CREDITS;
	}

	kref_get(&wdata->refcount);
	rc = cifs_call_async(server, &rqst, NULL, smb2_writev_callback, NULL,
			     wdata, flags, &wdata->credits);

	if (rc) {
		trace_smb3_write_err(0 /* no xid */, req->PersistentFileId,
				     tcon->tid, tcon->ses->Suid, wdata->offset,
				     wdata->bytes, rc);
		kref_put(&wdata->refcount, release);
		cifs_stats_fail_inc(tcon, SMB2_WRITE_HE);
	}

async_writev_out:
	cifs_small_buf_release(req);
	return rc;
}