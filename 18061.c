NTSTATUS open_streams_for_delete(connection_struct *conn,
					const char *fname)
{
	struct stream_struct *stream_info = NULL;
	files_struct **streams = NULL;
	int i;
	unsigned int num_streams = 0;
	TALLOC_CTX *frame = talloc_stackframe();
	NTSTATUS status;

	status = vfs_streaminfo(conn, NULL, fname, talloc_tos(),
				&num_streams, &stream_info);

	if (NT_STATUS_EQUAL(status, NT_STATUS_NOT_IMPLEMENTED)
	    || NT_STATUS_EQUAL(status, NT_STATUS_OBJECT_NAME_NOT_FOUND)) {
		DEBUG(10, ("no streams around\n"));
		TALLOC_FREE(frame);
		return NT_STATUS_OK;
	}

	if (!NT_STATUS_IS_OK(status)) {
		DEBUG(10, ("vfs_streaminfo failed: %s\n",
			   nt_errstr(status)));
		goto fail;
	}

	DEBUG(10, ("open_streams_for_delete found %d streams\n",
		   num_streams));

	if (num_streams == 0) {
		TALLOC_FREE(frame);
		return NT_STATUS_OK;
	}

	streams = talloc_array(talloc_tos(), files_struct *, num_streams);
	if (streams == NULL) {
		DEBUG(0, ("talloc failed\n"));
		status = NT_STATUS_NO_MEMORY;
		goto fail;
	}

	for (i=0; i<num_streams; i++) {
		struct smb_filename *smb_fname;

		if (strequal(stream_info[i].name, "::$DATA")) {
			streams[i] = NULL;
			continue;
		}

		smb_fname = synthetic_smb_fname(
			talloc_tos(), fname, stream_info[i].name, NULL);
		if (smb_fname == NULL) {
			status = NT_STATUS_NO_MEMORY;
			goto fail;
		}

		if (SMB_VFS_STAT(conn, smb_fname) == -1) {
			DEBUG(10, ("Unable to stat stream: %s\n",
				   smb_fname_str_dbg(smb_fname)));
		}

		status = SMB_VFS_CREATE_FILE(
			 conn,			/* conn */
			 NULL,			/* req */
			 0,			/* root_dir_fid */
			 smb_fname,		/* fname */
			 DELETE_ACCESS,		/* access_mask */
			 (FILE_SHARE_READ |	/* share_access */
			     FILE_SHARE_WRITE | FILE_SHARE_DELETE),
			 FILE_OPEN,		/* create_disposition*/
			 0, 			/* create_options */
			 FILE_ATTRIBUTE_NORMAL,	/* file_attributes */
			 0,			/* oplock_request */
			 0,			/* allocation_size */
			 NTCREATEX_OPTIONS_PRIVATE_STREAM_DELETE, /* private_flags */
			 NULL,			/* sd */
			 NULL,			/* ea_list */
			 &streams[i],		/* result */
			 NULL);			/* pinfo */

		if (!NT_STATUS_IS_OK(status)) {
			DEBUG(10, ("Could not open stream %s: %s\n",
				   smb_fname_str_dbg(smb_fname),
				   nt_errstr(status)));

			TALLOC_FREE(smb_fname);
			break;
		}
		TALLOC_FREE(smb_fname);
	}

	/*
	 * don't touch the variable "status" beyond this point :-)
	 */

	for (i -= 1 ; i >= 0; i--) {
		if (streams[i] == NULL) {
			continue;
		}

		DEBUG(10, ("Closing stream # %d, %s\n", i,
			   fsp_str_dbg(streams[i])));
		close_file(NULL, streams[i], NORMAL_CLOSE);
	}

 fail:
	TALLOC_FREE(frame);
	return status;
}