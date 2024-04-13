SMB2_open(const unsigned int xid, struct cifs_open_parms *oparms, __le16 *path,
	  __u8 *oplock, struct smb2_file_all_info *buf,
	  struct kvec *err_iov, int *buftype)
{
	struct smb_rqst rqst;
	struct smb2_create_rsp *rsp = NULL;
	struct TCP_Server_Info *server;
	struct cifs_tcon *tcon = oparms->tcon;
	struct cifs_ses *ses = tcon->ses;
	struct kvec iov[SMB2_CREATE_IOV_SIZE];
	struct kvec rsp_iov = {NULL, 0};
	int resp_buftype = CIFS_NO_BUFFER;
	int rc = 0;
	int flags = 0;

	cifs_dbg(FYI, "create/open\n");
	if (ses && (ses->server))
		server = ses->server;
	else
		return -EIO;

	if (smb3_encryption_required(tcon))
		flags |= CIFS_TRANSFORM_REQ;

	memset(&rqst, 0, sizeof(struct smb_rqst));
	memset(&iov, 0, sizeof(iov));
	rqst.rq_iov = iov;
	rqst.rq_nvec = SMB2_CREATE_IOV_SIZE;

	rc = SMB2_open_init(tcon, &rqst, oplock, oparms, path);
	if (rc)
		goto creat_exit;

	trace_smb3_open_enter(xid, tcon->tid, tcon->ses->Suid,
		oparms->create_options, oparms->desired_access);

	rc = cifs_send_recv(xid, ses, &rqst, &resp_buftype, flags,
			    &rsp_iov);
	rsp = (struct smb2_create_rsp *)rsp_iov.iov_base;

	if (rc != 0) {
		cifs_stats_fail_inc(tcon, SMB2_CREATE_HE);
		if (err_iov && rsp) {
			*err_iov = rsp_iov;
			*buftype = resp_buftype;
			resp_buftype = CIFS_NO_BUFFER;
			rsp = NULL;
		}
		trace_smb3_open_err(xid, tcon->tid, ses->Suid,
				    oparms->create_options, oparms->desired_access, rc);
		goto creat_exit;
	} else
		trace_smb3_open_done(xid, rsp->PersistentFileId, tcon->tid,
				     ses->Suid, oparms->create_options,
				     oparms->desired_access);

	atomic_inc(&tcon->num_remote_opens);
	oparms->fid->persistent_fid = rsp->PersistentFileId;
	oparms->fid->volatile_fid = rsp->VolatileFileId;
#ifdef CONFIG_CIFS_DEBUG2
	oparms->fid->mid = le64_to_cpu(rsp->sync_hdr.MessageId);
#endif /* CIFS_DEBUG2 */

	if (buf) {
		memcpy(buf, &rsp->CreationTime, 32);
		buf->AllocationSize = rsp->AllocationSize;
		buf->EndOfFile = rsp->EndofFile;
		buf->Attributes = rsp->FileAttributes;
		buf->NumberOfLinks = cpu_to_le32(1);
		buf->DeletePending = 0;
	}

	if (rsp->OplockLevel == SMB2_OPLOCK_LEVEL_LEASE)
		*oplock = smb2_parse_lease_state(server, rsp,
						 &oparms->fid->epoch,
						 oparms->fid->lease_key);
	else
		*oplock = rsp->OplockLevel;
creat_exit:
	SMB2_open_free(&rqst);
	free_rsp_buf(resp_buftype, rsp);
	return rc;
}