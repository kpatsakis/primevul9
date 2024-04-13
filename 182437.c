SMB2_open_init(struct cifs_tcon *tcon, struct smb_rqst *rqst, __u8 *oplock,
	       struct cifs_open_parms *oparms, __le16 *path)
{
	struct TCP_Server_Info *server = tcon->ses->server;
	struct smb2_create_req *req;
	unsigned int n_iov = 2;
	__u32 file_attributes = 0;
	int copy_size;
	int uni_path_len;
	unsigned int total_len;
	struct kvec *iov = rqst->rq_iov;
	__le16 *copy_path;
	int rc;

	rc = smb2_plain_req_init(SMB2_CREATE, tcon, (void **) &req, &total_len);
	if (rc)
		return rc;

	iov[0].iov_base = (char *)req;
	/* -1 since last byte is buf[0] which is sent below (path) */
	iov[0].iov_len = total_len - 1;

	if (oparms->create_options & CREATE_OPTION_READONLY)
		file_attributes |= ATTR_READONLY;
	if (oparms->create_options & CREATE_OPTION_SPECIAL)
		file_attributes |= ATTR_SYSTEM;

	req->ImpersonationLevel = IL_IMPERSONATION;
	req->DesiredAccess = cpu_to_le32(oparms->desired_access);
	/* File attributes ignored on open (used in create though) */
	req->FileAttributes = cpu_to_le32(file_attributes);
	req->ShareAccess = FILE_SHARE_ALL_LE;
	req->CreateDisposition = cpu_to_le32(oparms->disposition);
	req->CreateOptions = cpu_to_le32(oparms->create_options & CREATE_OPTIONS_MASK);
	req->NameOffset = cpu_to_le16(sizeof(struct smb2_create_req));

	/* [MS-SMB2] 2.2.13 NameOffset:
	 * If SMB2_FLAGS_DFS_OPERATIONS is set in the Flags field of
	 * the SMB2 header, the file name includes a prefix that will
	 * be processed during DFS name normalization as specified in
	 * section 3.3.5.9. Otherwise, the file name is relative to
	 * the share that is identified by the TreeId in the SMB2
	 * header.
	 */
	if (tcon->share_flags & SHI1005_FLAGS_DFS) {
		int name_len;

		req->sync_hdr.Flags |= SMB2_FLAGS_DFS_OPERATIONS;
		rc = alloc_path_with_tree_prefix(&copy_path, &copy_size,
						 &name_len,
						 tcon->treeName, path);
		if (rc)
			return rc;
		req->NameLength = cpu_to_le16(name_len * 2);
		uni_path_len = copy_size;
		path = copy_path;
	} else {
		uni_path_len = (2 * UniStrnlen((wchar_t *)path, PATH_MAX)) + 2;
		/* MUST set path len (NameLength) to 0 opening root of share */
		req->NameLength = cpu_to_le16(uni_path_len - 2);
		copy_size = uni_path_len;
		if (copy_size % 8 != 0)
			copy_size = roundup(copy_size, 8);
		copy_path = kzalloc(copy_size, GFP_KERNEL);
		if (!copy_path)
			return -ENOMEM;
		memcpy((char *)copy_path, (const char *)path,
		       uni_path_len);
		uni_path_len = copy_size;
		path = copy_path;
	}

	iov[1].iov_len = uni_path_len;
	iov[1].iov_base = path;

	if (!server->oplocks)
		*oplock = SMB2_OPLOCK_LEVEL_NONE;

	if (!(server->capabilities & SMB2_GLOBAL_CAP_LEASING) ||
	    *oplock == SMB2_OPLOCK_LEVEL_NONE)
		req->RequestedOplockLevel = *oplock;
	else if (!(server->capabilities & SMB2_GLOBAL_CAP_DIRECTORY_LEASING) &&
		  (oparms->create_options & CREATE_NOT_FILE))
		req->RequestedOplockLevel = *oplock; /* no srv lease support */
	else {
		rc = add_lease_context(server, iov, &n_iov,
				       oparms->fid->lease_key, oplock);
		if (rc)
			return rc;
	}

	if (*oplock == SMB2_OPLOCK_LEVEL_BATCH) {
		/* need to set Next field of lease context if we request it */
		if (server->capabilities & SMB2_GLOBAL_CAP_LEASING) {
			struct create_context *ccontext =
			    (struct create_context *)iov[n_iov-1].iov_base;
			ccontext->Next =
				cpu_to_le32(server->vals->create_lease_size);
		}

		rc = add_durable_context(iov, &n_iov, oparms,
					tcon->use_persistent);
		if (rc)
			return rc;
	}

	if (tcon->posix_extensions) {
		if (n_iov > 2) {
			struct create_context *ccontext =
			    (struct create_context *)iov[n_iov-1].iov_base;
			ccontext->Next =
				cpu_to_le32(iov[n_iov-1].iov_len);
		}

		rc = add_posix_context(iov, &n_iov, oparms->mode);
		if (rc)
			return rc;
	}

	if (tcon->snapshot_time) {
		cifs_dbg(FYI, "adding snapshot context\n");
		if (n_iov > 2) {
			struct create_context *ccontext =
			    (struct create_context *)iov[n_iov-1].iov_base;
			ccontext->Next =
				cpu_to_le32(iov[n_iov-1].iov_len);
		}

		rc = add_twarp_context(iov, &n_iov, tcon->snapshot_time);
		if (rc)
			return rc;
	}


	rqst->rq_nvec = n_iov;
	return 0;
}