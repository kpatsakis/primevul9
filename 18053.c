static NTSTATUS open_directory(connection_struct *conn,
			       struct smb_request *req,
			       struct smb_filename *smb_dname,
			       uint32 access_mask,
			       uint32 share_access,
			       uint32 create_disposition,
			       uint32 create_options,
			       uint32 file_attributes,
			       int *pinfo,
			       files_struct **result)
{
	files_struct *fsp = NULL;
	bool dir_existed = VALID_STAT(smb_dname->st) ? True : False;
	struct share_mode_lock *lck = NULL;
	NTSTATUS status;
	struct timespec mtimespec;
	int info = 0;

	if (is_ntfs_stream_smb_fname(smb_dname)) {
		DEBUG(2, ("open_directory: %s is a stream name!\n",
			  smb_fname_str_dbg(smb_dname)));
		return NT_STATUS_NOT_A_DIRECTORY;
	}

	if (!(file_attributes & FILE_FLAG_POSIX_SEMANTICS)) {
		/* Ensure we have a directory attribute. */
		file_attributes |= FILE_ATTRIBUTE_DIRECTORY;
	}

	DEBUG(5,("open_directory: opening directory %s, access_mask = 0x%x, "
		 "share_access = 0x%x create_options = 0x%x, "
		 "create_disposition = 0x%x, file_attributes = 0x%x\n",
		 smb_fname_str_dbg(smb_dname),
		 (unsigned int)access_mask,
		 (unsigned int)share_access,
		 (unsigned int)create_options,
		 (unsigned int)create_disposition,
		 (unsigned int)file_attributes));

	status = smbd_calculate_access_mask(conn, smb_dname, false,
					    access_mask, &access_mask);
	if (!NT_STATUS_IS_OK(status)) {
		DEBUG(10, ("open_directory: smbd_calculate_access_mask "
			"on file %s returned %s\n",
			smb_fname_str_dbg(smb_dname),
			nt_errstr(status)));
		return status;
	}

	if ((access_mask & SEC_FLAG_SYSTEM_SECURITY) &&
			!security_token_has_privilege(get_current_nttok(conn),
					SEC_PRIV_SECURITY)) {
		DEBUG(10, ("open_directory: open on %s "
			"failed - SEC_FLAG_SYSTEM_SECURITY denied.\n",
			smb_fname_str_dbg(smb_dname)));
		return NT_STATUS_PRIVILEGE_NOT_HELD;
	}

	switch( create_disposition ) {
		case FILE_OPEN:

			if (!dir_existed) {
				return NT_STATUS_OBJECT_NAME_NOT_FOUND;
			}

			info = FILE_WAS_OPENED;
			break;

		case FILE_CREATE:

			/* If directory exists error. If directory doesn't
			 * exist create. */

			if (dir_existed) {
				status = NT_STATUS_OBJECT_NAME_COLLISION;
				DEBUG(2, ("open_directory: unable to create "
					  "%s. Error was %s\n",
					  smb_fname_str_dbg(smb_dname),
					  nt_errstr(status)));
				return status;
			}

			status = mkdir_internal(conn, smb_dname,
						file_attributes);

			if (!NT_STATUS_IS_OK(status)) {
				DEBUG(2, ("open_directory: unable to create "
					  "%s. Error was %s\n",
					  smb_fname_str_dbg(smb_dname),
					  nt_errstr(status)));
				return status;
			}

			info = FILE_WAS_CREATED;
			break;

		case FILE_OPEN_IF:
			/*
			 * If directory exists open. If directory doesn't
			 * exist create.
			 */

			if (dir_existed) {
				status = NT_STATUS_OK;
				info = FILE_WAS_OPENED;
			} else {
				status = mkdir_internal(conn, smb_dname,
						file_attributes);

				if (NT_STATUS_IS_OK(status)) {
					info = FILE_WAS_CREATED;
				} else {
					/* Cope with create race. */
					if (!NT_STATUS_EQUAL(status,
							NT_STATUS_OBJECT_NAME_COLLISION)) {
						DEBUG(2, ("open_directory: unable to create "
							"%s. Error was %s\n",
							smb_fname_str_dbg(smb_dname),
							nt_errstr(status)));
						return status;
					}
					info = FILE_WAS_OPENED;
				}
			}

			break;

		case FILE_SUPERSEDE:
		case FILE_OVERWRITE:
		case FILE_OVERWRITE_IF:
		default:
			DEBUG(5,("open_directory: invalid create_disposition "
				 "0x%x for directory %s\n",
				 (unsigned int)create_disposition,
				 smb_fname_str_dbg(smb_dname)));
			return NT_STATUS_INVALID_PARAMETER;
	}

	if(!S_ISDIR(smb_dname->st.st_ex_mode)) {
		DEBUG(5,("open_directory: %s is not a directory !\n",
			 smb_fname_str_dbg(smb_dname)));
		return NT_STATUS_NOT_A_DIRECTORY;
	}

	if (info == FILE_WAS_OPENED) {
		status = smbd_check_access_rights(conn,
						smb_dname,
						false,
						access_mask);
		if (!NT_STATUS_IS_OK(status)) {
			DEBUG(10, ("open_directory: smbd_check_access_rights on "
				"file %s failed with %s\n",
				smb_fname_str_dbg(smb_dname),
				nt_errstr(status)));
			return status;
		}
	}

	status = file_new(req, conn, &fsp);
	if(!NT_STATUS_IS_OK(status)) {
		return status;
	}

	/*
	 * Setup the files_struct for it.
	 */

	fsp->file_id = vfs_file_id_from_sbuf(conn, &smb_dname->st);
	fsp->vuid = req ? req->vuid : UID_FIELD_INVALID;
	fsp->file_pid = req ? req->smbpid : 0;
	fsp->can_lock = False;
	fsp->can_read = False;
	fsp->can_write = False;

	fsp->share_access = share_access;
	fsp->fh->private_options = 0;
	/*
	 * According to Samba4, SEC_FILE_READ_ATTRIBUTE is always granted,
	 */
	fsp->access_mask = access_mask | FILE_READ_ATTRIBUTES;
	fsp->print_file = NULL;
	fsp->modified = False;
	fsp->oplock_type = NO_OPLOCK;
	fsp->sent_oplock_break = NO_BREAK_SENT;
	fsp->is_directory = True;
	fsp->posix_open = (file_attributes & FILE_FLAG_POSIX_SEMANTICS) ? True : False;
	status = fsp_set_smb_fname(fsp, smb_dname);
	if (!NT_STATUS_IS_OK(status)) {
		file_free(req, fsp);
		return status;
	}

	mtimespec = smb_dname->st.st_ex_mtime;

#ifdef O_DIRECTORY
	status = fd_open(conn, fsp, O_RDONLY|O_DIRECTORY, 0);
#else
	/* POSIX allows us to open a directory with O_RDONLY. */
	status = fd_open(conn, fsp, O_RDONLY, 0);
#endif
	if (!NT_STATUS_IS_OK(status)) {
		DEBUG(5, ("open_directory: Could not open fd for "
			"%s (%s)\n",
			smb_fname_str_dbg(smb_dname),
			nt_errstr(status)));
		file_free(req, fsp);
		return status;
	}

	status = vfs_stat_fsp(fsp);
	if (!NT_STATUS_IS_OK(status)) {
		fd_close(fsp);
		file_free(req, fsp);
		return status;
	}

	/* Ensure there was no race condition. */
	if (!check_same_stat(&smb_dname->st, &fsp->fsp_name->st)) {
		DEBUG(5,("open_directory: stat struct differs for "
			"directory %s.\n",
			smb_fname_str_dbg(smb_dname)));
		fd_close(fsp);
		file_free(req, fsp);
		return NT_STATUS_ACCESS_DENIED;
	}

	lck = get_share_mode_lock(talloc_tos(), fsp->file_id,
				  conn->connectpath, smb_dname,
				  &mtimespec);

	if (lck == NULL) {
		DEBUG(0, ("open_directory: Could not get share mode lock for "
			  "%s\n", smb_fname_str_dbg(smb_dname)));
		fd_close(fsp);
		file_free(req, fsp);
		return NT_STATUS_SHARING_VIOLATION;
	}

	if (has_delete_on_close(lck, fsp->name_hash)) {
		TALLOC_FREE(lck);
		fd_close(fsp);
		file_free(req, fsp);
		return NT_STATUS_DELETE_PENDING;
	}

	status = open_mode_check(conn, lck,
				 access_mask, share_access);

	if (!NT_STATUS_IS_OK(status)) {
		TALLOC_FREE(lck);
		fd_close(fsp);
		file_free(req, fsp);
		return status;
	}

	if (!set_share_mode(lck, fsp, get_current_uid(conn),
			    req ? req->mid : 0, NO_OPLOCK)) {
		TALLOC_FREE(lck);
		fd_close(fsp);
		file_free(req, fsp);
		return NT_STATUS_NO_MEMORY;
	}

	/* For directories the delete on close bit at open time seems
	   always to be honored on close... See test 19 in Samba4 BASE-DELETE. */
	if (create_options & FILE_DELETE_ON_CLOSE) {
		status = can_set_delete_on_close(fsp, 0);
		if (!NT_STATUS_IS_OK(status) && !NT_STATUS_EQUAL(status, NT_STATUS_DIRECTORY_NOT_EMPTY)) {
			del_share_mode(lck, fsp);
			TALLOC_FREE(lck);
			fd_close(fsp);
			file_free(req, fsp);
			return status;
		}

		if (NT_STATUS_IS_OK(status)) {
			/* Note that here we set the *inital* delete on close flag,
			   not the regular one. The magic gets handled in close. */
			fsp->initial_delete_on_close = True;
		}
	}

	TALLOC_FREE(lck);

	if (pinfo) {
		*pinfo = info;
	}

	*result = fsp;
	return NT_STATUS_OK;
}