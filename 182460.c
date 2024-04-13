smb2_new_read_req(void **buf, unsigned int *total_len,
	struct cifs_io_parms *io_parms, struct cifs_readdata *rdata,
	unsigned int remaining_bytes, int request_type)
{
	int rc = -EACCES;
	struct smb2_read_plain_req *req = NULL;
	struct smb2_sync_hdr *shdr;
	struct TCP_Server_Info *server;

	rc = smb2_plain_req_init(SMB2_READ, io_parms->tcon, (void **) &req,
				 total_len);
	if (rc)
		return rc;

	server = io_parms->tcon->ses->server;
	if (server == NULL)
		return -ECONNABORTED;

	shdr = &req->sync_hdr;
	shdr->ProcessId = cpu_to_le32(io_parms->pid);

	req->PersistentFileId = io_parms->persistent_fid;
	req->VolatileFileId = io_parms->volatile_fid;
	req->ReadChannelInfoOffset = 0; /* reserved */
	req->ReadChannelInfoLength = 0; /* reserved */
	req->Channel = 0; /* reserved */
	req->MinimumCount = 0;
	req->Length = cpu_to_le32(io_parms->length);
	req->Offset = cpu_to_le64(io_parms->offset);

	trace_smb3_read_enter(0 /* xid */,
			io_parms->persistent_fid,
			io_parms->tcon->tid, io_parms->tcon->ses->Suid,
			io_parms->offset, io_parms->length);
#ifdef CONFIG_CIFS_SMB_DIRECT
	/*
	 * If we want to do a RDMA write, fill in and append
	 * smbd_buffer_descriptor_v1 to the end of read request
	 */
	if (server->rdma && rdata && !server->sign &&
		rdata->bytes >= server->smbd_conn->rdma_readwrite_threshold) {

		struct smbd_buffer_descriptor_v1 *v1;
		bool need_invalidate =
			io_parms->tcon->ses->server->dialect == SMB30_PROT_ID;

		rdata->mr = smbd_register_mr(
				server->smbd_conn, rdata->pages,
				rdata->nr_pages, rdata->page_offset,
				rdata->tailsz, true, need_invalidate);
		if (!rdata->mr)
			return -ENOBUFS;

		req->Channel = SMB2_CHANNEL_RDMA_V1_INVALIDATE;
		if (need_invalidate)
			req->Channel = SMB2_CHANNEL_RDMA_V1;
		req->ReadChannelInfoOffset =
			cpu_to_le16(offsetof(struct smb2_read_plain_req, Buffer));
		req->ReadChannelInfoLength =
			cpu_to_le16(sizeof(struct smbd_buffer_descriptor_v1));
		v1 = (struct smbd_buffer_descriptor_v1 *) &req->Buffer[0];
		v1->offset = cpu_to_le64(rdata->mr->mr->iova);
		v1->token = cpu_to_le32(rdata->mr->mr->rkey);
		v1->length = cpu_to_le32(rdata->mr->mr->length);

		*total_len += sizeof(*v1) - 1;
	}
#endif
	if (request_type & CHAINED_REQUEST) {
		if (!(request_type & END_OF_CHAIN)) {
			/* next 8-byte aligned request */
			*total_len = DIV_ROUND_UP(*total_len, 8) * 8;
			shdr->NextCommand = cpu_to_le32(*total_len);
		} else /* END_OF_CHAIN */
			shdr->NextCommand = 0;
		if (request_type & RELATED_REQUEST) {
			shdr->Flags |= SMB2_FLAGS_RELATED_OPERATIONS;
			/*
			 * Related requests use info from previous read request
			 * in chain.
			 */
			shdr->SessionId = 0xFFFFFFFF;
			shdr->TreeId = 0xFFFFFFFF;
			req->PersistentFileId = 0xFFFFFFFF;
			req->VolatileFileId = 0xFFFFFFFF;
		}
	}
	if (remaining_bytes > io_parms->length)
		req->RemainingBytes = cpu_to_le32(remaining_bytes);
	else
		req->RemainingBytes = 0;

	*buf = req;
	return rc;
}