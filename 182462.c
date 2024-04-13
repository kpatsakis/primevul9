int smb3_validate_negotiate(const unsigned int xid, struct cifs_tcon *tcon)
{
	int rc;
	struct validate_negotiate_info_req *pneg_inbuf;
	struct validate_negotiate_info_rsp *pneg_rsp = NULL;
	u32 rsplen;
	u32 inbuflen; /* max of 4 dialects */

	cifs_dbg(FYI, "validate negotiate\n");

	/* In SMB3.11 preauth integrity supersedes validate negotiate */
	if (tcon->ses->server->dialect == SMB311_PROT_ID)
		return 0;

	/*
	 * validation ioctl must be signed, so no point sending this if we
	 * can not sign it (ie are not known user).  Even if signing is not
	 * required (enabled but not negotiated), in those cases we selectively
	 * sign just this, the first and only signed request on a connection.
	 * Having validation of negotiate info  helps reduce attack vectors.
	 */
	if (tcon->ses->session_flags & SMB2_SESSION_FLAG_IS_GUEST)
		return 0; /* validation requires signing */

	if (tcon->ses->user_name == NULL) {
		cifs_dbg(FYI, "Can't validate negotiate: null user mount\n");
		return 0; /* validation requires signing */
	}

	if (tcon->ses->session_flags & SMB2_SESSION_FLAG_IS_NULL)
		cifs_dbg(VFS, "Unexpected null user (anonymous) auth flag sent by server\n");

	pneg_inbuf = kmalloc(sizeof(*pneg_inbuf), GFP_NOFS);
	if (!pneg_inbuf)
		return -ENOMEM;

	pneg_inbuf->Capabilities =
			cpu_to_le32(tcon->ses->server->vals->req_capabilities);
	memcpy(pneg_inbuf->Guid, tcon->ses->server->client_guid,
					SMB2_CLIENT_GUID_SIZE);

	if (tcon->ses->sign)
		pneg_inbuf->SecurityMode =
			cpu_to_le16(SMB2_NEGOTIATE_SIGNING_REQUIRED);
	else if (global_secflags & CIFSSEC_MAY_SIGN)
		pneg_inbuf->SecurityMode =
			cpu_to_le16(SMB2_NEGOTIATE_SIGNING_ENABLED);
	else
		pneg_inbuf->SecurityMode = 0;


	if (strcmp(tcon->ses->server->vals->version_string,
		SMB3ANY_VERSION_STRING) == 0) {
		pneg_inbuf->Dialects[0] = cpu_to_le16(SMB30_PROT_ID);
		pneg_inbuf->Dialects[1] = cpu_to_le16(SMB302_PROT_ID);
		pneg_inbuf->DialectCount = cpu_to_le16(2);
		/* structure is big enough for 3 dialects, sending only 2 */
		inbuflen = sizeof(*pneg_inbuf) -
				(2 * sizeof(pneg_inbuf->Dialects[0]));
	} else if (strcmp(tcon->ses->server->vals->version_string,
		SMBDEFAULT_VERSION_STRING) == 0) {
		pneg_inbuf->Dialects[0] = cpu_to_le16(SMB21_PROT_ID);
		pneg_inbuf->Dialects[1] = cpu_to_le16(SMB30_PROT_ID);
		pneg_inbuf->Dialects[2] = cpu_to_le16(SMB302_PROT_ID);
		pneg_inbuf->Dialects[3] = cpu_to_le16(SMB311_PROT_ID);
		pneg_inbuf->DialectCount = cpu_to_le16(4);
		/* structure is big enough for 3 dialects */
		inbuflen = sizeof(*pneg_inbuf);
	} else {
		/* otherwise specific dialect was requested */
		pneg_inbuf->Dialects[0] =
			cpu_to_le16(tcon->ses->server->vals->protocol_id);
		pneg_inbuf->DialectCount = cpu_to_le16(1);
		/* structure is big enough for 3 dialects, sending only 1 */
		inbuflen = sizeof(*pneg_inbuf) -
				sizeof(pneg_inbuf->Dialects[0]) * 2;
	}

	rc = SMB2_ioctl(xid, tcon, NO_FILE_ID, NO_FILE_ID,
		FSCTL_VALIDATE_NEGOTIATE_INFO, true /* is_fsctl */,
		(char *)pneg_inbuf, inbuflen, CIFSMaxBufSize,
		(char **)&pneg_rsp, &rsplen);
	if (rc == -EOPNOTSUPP) {
		/*
		 * Old Windows versions or Netapp SMB server can return
		 * not supported error. Client should accept it.
		 */
		cifs_dbg(VFS, "Server does not support validate negotiate\n");
		return 0;
	} else if (rc != 0) {
		cifs_dbg(VFS, "validate protocol negotiate failed: %d\n", rc);
		rc = -EIO;
		goto out_free_inbuf;
	}

	rc = -EIO;
	if (rsplen != sizeof(*pneg_rsp)) {
		cifs_dbg(VFS, "invalid protocol negotiate response size: %d\n",
			 rsplen);

		/* relax check since Mac returns max bufsize allowed on ioctl */
		if (rsplen > CIFSMaxBufSize || rsplen < sizeof(*pneg_rsp))
			goto out_free_rsp;
	}

	/* check validate negotiate info response matches what we got earlier */
	if (pneg_rsp->Dialect != cpu_to_le16(tcon->ses->server->dialect))
		goto vneg_out;

	if (pneg_rsp->SecurityMode != cpu_to_le16(tcon->ses->server->sec_mode))
		goto vneg_out;

	/* do not validate server guid because not saved at negprot time yet */

	if ((le32_to_cpu(pneg_rsp->Capabilities) | SMB2_NT_FIND |
	      SMB2_LARGE_FILES) != tcon->ses->server->capabilities)
		goto vneg_out;

	/* validate negotiate successful */
	rc = 0;
	cifs_dbg(FYI, "validate negotiate info successful\n");
	goto out_free_rsp;

vneg_out:
	cifs_dbg(VFS, "protocol revalidation - security settings mismatch\n");
out_free_rsp:
	kfree(pneg_rsp);
out_free_inbuf:
	kfree(pneg_inbuf);
	return rc;
}