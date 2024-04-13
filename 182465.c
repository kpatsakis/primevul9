SMB2_negotiate(const unsigned int xid, struct cifs_ses *ses)
{
	struct smb_rqst rqst;
	struct smb2_negotiate_req *req;
	struct smb2_negotiate_rsp *rsp;
	struct kvec iov[1];
	struct kvec rsp_iov;
	int rc = 0;
	int resp_buftype;
	struct TCP_Server_Info *server = ses->server;
	int blob_offset, blob_length;
	char *security_blob;
	int flags = CIFS_NEG_OP;
	unsigned int total_len;

	cifs_dbg(FYI, "Negotiate protocol\n");

	if (!server) {
		WARN(1, "%s: server is NULL!\n", __func__);
		return -EIO;
	}

	rc = smb2_plain_req_init(SMB2_NEGOTIATE, NULL, (void **) &req, &total_len);
	if (rc)
		return rc;

	req->sync_hdr.SessionId = 0;

	memset(server->preauth_sha_hash, 0, SMB2_PREAUTH_HASH_SIZE);
	memset(ses->preauth_sha_hash, 0, SMB2_PREAUTH_HASH_SIZE);

	if (strcmp(ses->server->vals->version_string,
		   SMB3ANY_VERSION_STRING) == 0) {
		req->Dialects[0] = cpu_to_le16(SMB30_PROT_ID);
		req->Dialects[1] = cpu_to_le16(SMB302_PROT_ID);
		req->DialectCount = cpu_to_le16(2);
		total_len += 4;
	} else if (strcmp(ses->server->vals->version_string,
		   SMBDEFAULT_VERSION_STRING) == 0) {
		req->Dialects[0] = cpu_to_le16(SMB21_PROT_ID);
		req->Dialects[1] = cpu_to_le16(SMB30_PROT_ID);
		req->Dialects[2] = cpu_to_le16(SMB302_PROT_ID);
		req->Dialects[3] = cpu_to_le16(SMB311_PROT_ID);
		req->DialectCount = cpu_to_le16(4);
		total_len += 8;
	} else {
		/* otherwise send specific dialect */
		req->Dialects[0] = cpu_to_le16(ses->server->vals->protocol_id);
		req->DialectCount = cpu_to_le16(1);
		total_len += 2;
	}

	/* only one of SMB2 signing flags may be set in SMB2 request */
	if (ses->sign)
		req->SecurityMode = cpu_to_le16(SMB2_NEGOTIATE_SIGNING_REQUIRED);
	else if (global_secflags & CIFSSEC_MAY_SIGN)
		req->SecurityMode = cpu_to_le16(SMB2_NEGOTIATE_SIGNING_ENABLED);
	else
		req->SecurityMode = 0;

	req->Capabilities = cpu_to_le32(ses->server->vals->req_capabilities);

	/* ClientGUID must be zero for SMB2.02 dialect */
	if (ses->server->vals->protocol_id == SMB20_PROT_ID)
		memset(req->ClientGUID, 0, SMB2_CLIENT_GUID_SIZE);
	else {
		memcpy(req->ClientGUID, server->client_guid,
			SMB2_CLIENT_GUID_SIZE);
		if ((ses->server->vals->protocol_id == SMB311_PROT_ID) ||
		    (strcmp(ses->server->vals->version_string,
		     SMBDEFAULT_VERSION_STRING) == 0))
			assemble_neg_contexts(req, &total_len);
	}
	iov[0].iov_base = (char *)req;
	iov[0].iov_len = total_len;

	memset(&rqst, 0, sizeof(struct smb_rqst));
	rqst.rq_iov = iov;
	rqst.rq_nvec = 1;

	rc = cifs_send_recv(xid, ses, &rqst, &resp_buftype, flags, &rsp_iov);
	cifs_small_buf_release(req);
	rsp = (struct smb2_negotiate_rsp *)rsp_iov.iov_base;
	/*
	 * No tcon so can't do
	 * cifs_stats_inc(&tcon->stats.smb2_stats.smb2_com_fail[SMB2...]);
	 */
	if (rc == -EOPNOTSUPP) {
		cifs_dbg(VFS, "Dialect not supported by server. Consider "
			"specifying vers=1.0 or vers=2.0 on mount for accessing"
			" older servers\n");
		goto neg_exit;
	} else if (rc != 0)
		goto neg_exit;

	if (strcmp(ses->server->vals->version_string,
		   SMB3ANY_VERSION_STRING) == 0) {
		if (rsp->DialectRevision == cpu_to_le16(SMB20_PROT_ID)) {
			cifs_dbg(VFS,
				"SMB2 dialect returned but not requested\n");
			return -EIO;
		} else if (rsp->DialectRevision == cpu_to_le16(SMB21_PROT_ID)) {
			cifs_dbg(VFS,
				"SMB2.1 dialect returned but not requested\n");
			return -EIO;
		}
	} else if (strcmp(ses->server->vals->version_string,
		   SMBDEFAULT_VERSION_STRING) == 0) {
		if (rsp->DialectRevision == cpu_to_le16(SMB20_PROT_ID)) {
			cifs_dbg(VFS,
				"SMB2 dialect returned but not requested\n");
			return -EIO;
		} else if (rsp->DialectRevision == cpu_to_le16(SMB21_PROT_ID)) {
			/* ops set to 3.0 by default for default so update */
			ses->server->ops = &smb21_operations;
			ses->server->vals = &smb21_values;
		} else if (rsp->DialectRevision == cpu_to_le16(SMB311_PROT_ID)) {
			ses->server->ops = &smb311_operations;
			ses->server->vals = &smb311_values;
		}
	} else if (le16_to_cpu(rsp->DialectRevision) !=
				ses->server->vals->protocol_id) {
		/* if requested single dialect ensure returned dialect matched */
		cifs_dbg(VFS, "Illegal 0x%x dialect returned: not requested\n",
			le16_to_cpu(rsp->DialectRevision));
		return -EIO;
	}

	cifs_dbg(FYI, "mode 0x%x\n", rsp->SecurityMode);

	if (rsp->DialectRevision == cpu_to_le16(SMB20_PROT_ID))
		cifs_dbg(FYI, "negotiated smb2.0 dialect\n");
	else if (rsp->DialectRevision == cpu_to_le16(SMB21_PROT_ID))
		cifs_dbg(FYI, "negotiated smb2.1 dialect\n");
	else if (rsp->DialectRevision == cpu_to_le16(SMB30_PROT_ID))
		cifs_dbg(FYI, "negotiated smb3.0 dialect\n");
	else if (rsp->DialectRevision == cpu_to_le16(SMB302_PROT_ID))
		cifs_dbg(FYI, "negotiated smb3.02 dialect\n");
	else if (rsp->DialectRevision == cpu_to_le16(SMB311_PROT_ID))
		cifs_dbg(FYI, "negotiated smb3.1.1 dialect\n");
	else {
		cifs_dbg(VFS, "Illegal dialect returned by server 0x%x\n",
			 le16_to_cpu(rsp->DialectRevision));
		rc = -EIO;
		goto neg_exit;
	}
	server->dialect = le16_to_cpu(rsp->DialectRevision);

	/*
	 * Keep a copy of the hash after negprot. This hash will be
	 * the starting hash value for all sessions made from this
	 * server.
	 */
	memcpy(server->preauth_sha_hash, ses->preauth_sha_hash,
	       SMB2_PREAUTH_HASH_SIZE);

	/* SMB2 only has an extended negflavor */
	server->negflavor = CIFS_NEGFLAVOR_EXTENDED;
	/* set it to the maximum buffer size value we can send with 1 credit */
	server->maxBuf = min_t(unsigned int, le32_to_cpu(rsp->MaxTransactSize),
			       SMB2_MAX_BUFFER_SIZE);
	server->max_read = le32_to_cpu(rsp->MaxReadSize);
	server->max_write = le32_to_cpu(rsp->MaxWriteSize);
	server->sec_mode = le16_to_cpu(rsp->SecurityMode);
	if ((server->sec_mode & SMB2_SEC_MODE_FLAGS_ALL) != server->sec_mode)
		cifs_dbg(FYI, "Server returned unexpected security mode 0x%x\n",
				server->sec_mode);
	server->capabilities = le32_to_cpu(rsp->Capabilities);
	/* Internal types */
	server->capabilities |= SMB2_NT_FIND | SMB2_LARGE_FILES;

	security_blob = smb2_get_data_area_len(&blob_offset, &blob_length,
					       (struct smb2_sync_hdr *)rsp);
	/*
	 * See MS-SMB2 section 2.2.4: if no blob, client picks default which
	 * for us will be
	 *	ses->sectype = RawNTLMSSP;
	 * but for time being this is our only auth choice so doesn't matter.
	 * We just found a server which sets blob length to zero expecting raw.
	 */
	if (blob_length == 0) {
		cifs_dbg(FYI, "missing security blob on negprot\n");
		server->sec_ntlmssp = true;
	}

	rc = cifs_enable_signing(server, ses->sign);
	if (rc)
		goto neg_exit;
	if (blob_length) {
		rc = decode_negTokenInit(security_blob, blob_length, server);
		if (rc == 1)
			rc = 0;
		else if (rc == 0)
			rc = -EIO;
	}

	if (rsp->DialectRevision == cpu_to_le16(SMB311_PROT_ID)) {
		if (rsp->NegotiateContextCount)
			rc = smb311_decode_neg_context(rsp, server,
						       rsp_iov.iov_len);
		else
			cifs_dbg(VFS, "Missing expected negotiate contexts\n");
	}
neg_exit:
	free_rsp_buf(resp_buftype, rsp);
	return rc;
}