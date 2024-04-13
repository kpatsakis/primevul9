NTSTATUS create_file_default(connection_struct *conn,
			     struct smb_request *req,
			     uint16_t root_dir_fid,
			     struct smb_filename *smb_fname,
			     uint32_t access_mask,
			     uint32_t share_access,
			     uint32_t create_disposition,
			     uint32_t create_options,
			     uint32_t file_attributes,
			     uint32_t oplock_request,
			     uint64_t allocation_size,
			     uint32_t private_flags,
			     struct security_descriptor *sd,
			     struct ea_list *ea_list,
			     files_struct **result,
			     int *pinfo)
{
	int info = FILE_WAS_OPENED;
	files_struct *fsp = NULL;
	NTSTATUS status;
	bool stream_name = false;

	DEBUG(10,("create_file: access_mask = 0x%x "
		  "file_attributes = 0x%x, share_access = 0x%x, "
		  "create_disposition = 0x%x create_options = 0x%x "
		  "oplock_request = 0x%x "
		  "private_flags = 0x%x "
		  "root_dir_fid = 0x%x, ea_list = 0x%p, sd = 0x%p, "
		  "fname = %s\n",
		  (unsigned int)access_mask,
		  (unsigned int)file_attributes,
		  (unsigned int)share_access,
		  (unsigned int)create_disposition,
		  (unsigned int)create_options,
		  (unsigned int)oplock_request,
		  (unsigned int)private_flags,
		  (unsigned int)root_dir_fid,
		  ea_list, sd, smb_fname_str_dbg(smb_fname)));

	/*
	 * Calculate the filename from the root_dir_if if necessary.
	 */

	if (root_dir_fid != 0) {
		struct smb_filename *smb_fname_out = NULL;
		status = get_relative_fid_filename(conn, req, root_dir_fid,
						   smb_fname, &smb_fname_out);
		if (!NT_STATUS_IS_OK(status)) {
			goto fail;
		}
		smb_fname = smb_fname_out;
	}

	/*
	 * Check to see if this is a mac fork of some kind.
	 */

	stream_name = is_ntfs_stream_smb_fname(smb_fname);
	if (stream_name) {
		enum FAKE_FILE_TYPE fake_file_type;

		fake_file_type = is_fake_file(smb_fname);

		if (fake_file_type != FAKE_FILE_TYPE_NONE) {

			/*
			 * Here we go! support for changing the disk quotas
			 * --metze
			 *
			 * We need to fake up to open this MAGIC QUOTA file
			 * and return a valid FID.
			 *
			 * w2k close this file directly after openening xp
			 * also tries a QUERY_FILE_INFO on the file and then
			 * close it
			 */
			status = open_fake_file(req, conn, req->vuid,
						fake_file_type, smb_fname,
						access_mask, &fsp);
			if (!NT_STATUS_IS_OK(status)) {
				goto fail;
			}

			ZERO_STRUCT(smb_fname->st);
			goto done;
		}

		if (!(conn->fs_capabilities & FILE_NAMED_STREAMS)) {
			status = NT_STATUS_OBJECT_NAME_NOT_FOUND;
			goto fail;
		}
	}

	if (is_ntfs_default_stream_smb_fname(smb_fname)) {
		int ret;
		smb_fname->stream_name = NULL;
		/* We have to handle this error here. */
		if (create_options & FILE_DIRECTORY_FILE) {
			status = NT_STATUS_NOT_A_DIRECTORY;
			goto fail;
		}
		if (lp_posix_pathnames()) {
			ret = SMB_VFS_LSTAT(conn, smb_fname);
		} else {
			ret = SMB_VFS_STAT(conn, smb_fname);
		}

		if (ret == 0 && VALID_STAT_OF_DIR(smb_fname->st)) {
			status = NT_STATUS_FILE_IS_A_DIRECTORY;
			goto fail;
		}
	}

	status = create_file_unixpath(
		conn, req, smb_fname, access_mask, share_access,
		create_disposition, create_options, file_attributes,
		oplock_request, allocation_size, private_flags,
		sd, ea_list,
		&fsp, &info);

	if (!NT_STATUS_IS_OK(status)) {
		goto fail;
	}

 done:
	DEBUG(10, ("create_file: info=%d\n", info));

	*result = fsp;
	if (pinfo != NULL) {
		*pinfo = info;
	}
	return NT_STATUS_OK;

 fail:
	DEBUG(10, ("create_file: %s\n", nt_errstr(status)));

	if (fsp != NULL) {
		close_file(req, fsp, ERROR_CLOSE);
		fsp = NULL;
	}
	return status;
}