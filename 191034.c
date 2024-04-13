void init_smb_request(struct smb_request *req,
			const uint8 *inbuf,
			size_t unread_bytes,
			bool encrypted)
{
	struct smbd_server_connection *sconn = smbd_server_conn;
	size_t req_size = smb_len(inbuf) + 4;
	/* Ensure we have at least smb_size bytes. */
	if (req_size < smb_size) {
		DEBUG(0,("init_smb_request: invalid request size %u\n",
			(unsigned int)req_size ));
		exit_server_cleanly("Invalid SMB request");
	}
	req->cmd    = CVAL(inbuf, smb_com);
	req->flags2 = SVAL(inbuf, smb_flg2);
	req->smbpid = SVAL(inbuf, smb_pid);
	req->mid    = SVAL(inbuf, smb_mid);
	req->seqnum = 0;
	req->vuid   = SVAL(inbuf, smb_uid);
	req->tid    = SVAL(inbuf, smb_tid);
	req->wct    = CVAL(inbuf, smb_wct);
	req->vwv    = (uint16_t *)(inbuf+smb_vwv);
	req->buflen = smb_buflen(inbuf);
	req->buf    = (const uint8_t *)smb_buf(inbuf);
	req->unread_bytes = unread_bytes;
	req->encrypted = encrypted;
	req->conn = conn_find(sconn,req->tid);
	req->chain_fsp = NULL;
	req->chain_outbuf = NULL;
	smb_init_perfcount_data(&req->pcd);

	/* Ensure we have at least wct words and 2 bytes of bcc. */
	if (smb_size + req->wct*2 > req_size) {
		DEBUG(0,("init_smb_request: invalid wct number %u (size %u)\n",
			(unsigned int)req->wct,
			(unsigned int)req_size));
		exit_server_cleanly("Invalid SMB request");
	}
	/* Ensure bcc is correct. */
	if (((uint8 *)smb_buf(inbuf)) + req->buflen > inbuf + req_size) {
		DEBUG(0,("init_smb_request: invalid bcc number %u "
			"(wct = %u, size %u)\n",
			(unsigned int)req->buflen,
			(unsigned int)req->wct,
			(unsigned int)req_size));
		exit_server_cleanly("Invalid SMB request");
	}

	req->outbuf = NULL;
}