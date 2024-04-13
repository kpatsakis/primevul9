static NTSTATUS open_file(files_struct *fsp,
			  connection_struct *conn,
			  struct smb_request *req,
			  const char *parent_dir,
			  int flags,
			  mode_t unx_mode,
			  uint32 access_mask, /* client requested access mask. */
			  uint32 open_access_mask, /* what we're actually using in the open. */
			  bool *p_file_created)
{
	struct smb_filename *smb_fname = fsp->fsp_name;
	NTSTATUS status = NT_STATUS_OK;
	int accmode = (flags & O_ACCMODE);
	int local_flags = flags;
	bool file_existed = VALID_STAT(fsp->fsp_name->st);

	fsp->fh->fd = -1;
	errno = EPERM;

	/* Check permissions */

	/*
	 * This code was changed after seeing a client open request 
	 * containing the open mode of (DENY_WRITE/read-only) with
	 * the 'create if not exist' bit set. The previous code
	 * would fail to open the file read only on a read-only share
	 * as it was checking the flags parameter  directly against O_RDONLY,
	 * this was failing as the flags parameter was set to O_RDONLY|O_CREAT.
	 * JRA.
	 */

	if (!CAN_WRITE(conn)) {
		/* It's a read-only share - fail if we wanted to write. */
		if(accmode != O_RDONLY || (flags & O_TRUNC) || (flags & O_APPEND)) {
			DEBUG(3,("Permission denied opening %s\n",
				 smb_fname_str_dbg(smb_fname)));
			return NT_STATUS_ACCESS_DENIED;
		}
		if (flags & O_CREAT) {
			/* We don't want to write - but we must make sure that
			   O_CREAT doesn't create the file if we have write
			   access into the directory.
			*/
			flags &= ~(O_CREAT|O_EXCL);
			local_flags &= ~(O_CREAT|O_EXCL);
		}
	}

	/*
	 * This little piece of insanity is inspired by the
	 * fact that an NT client can open a file for O_RDONLY,
	 * but set the create disposition to FILE_EXISTS_TRUNCATE.
	 * If the client *can* write to the file, then it expects to
	 * truncate the file, even though it is opening for readonly.
	 * Quicken uses this stupid trick in backup file creation...
	 * Thanks *greatly* to "David W. Chapman Jr." <dwcjr@inethouston.net>
	 * for helping track this one down. It didn't bite us in 2.0.x
	 * as we always opened files read-write in that release. JRA.
	 */

	if ((accmode == O_RDONLY) && ((flags & O_TRUNC) == O_TRUNC)) {
		DEBUG(10,("open_file: truncate requested on read-only open "
			  "for file %s\n", smb_fname_str_dbg(smb_fname)));
		local_flags = (flags & ~O_ACCMODE)|O_RDWR;
	}

	if ((open_access_mask & (FILE_READ_DATA|FILE_WRITE_DATA|FILE_APPEND_DATA|FILE_EXECUTE)) ||
	    (!file_existed && (local_flags & O_CREAT)) ||
	    ((local_flags & O_TRUNC) == O_TRUNC) ) {
		const char *wild;
		int ret;

#if defined(O_NONBLOCK) && defined(S_ISFIFO)
		/*
		 * We would block on opening a FIFO with no one else on the
		 * other end. Do what we used to do and add O_NONBLOCK to the
		 * open flags. JRA.
		 */

		if (file_existed && S_ISFIFO(smb_fname->st.st_ex_mode)) {
			local_flags &= ~O_TRUNC; /* Can't truncate a FIFO. */
			local_flags |= O_NONBLOCK;
		}
#endif

		/* Don't create files with Microsoft wildcard characters. */
		if (fsp->base_fsp) {
			/*
			 * wildcard characters are allowed in stream names
			 * only test the basefilename
			 */
			wild = fsp->base_fsp->fsp_name->base_name;
		} else {
			wild = smb_fname->base_name;
		}
		if ((local_flags & O_CREAT) && !file_existed &&
		    ms_has_wild(wild))  {
			return NT_STATUS_OBJECT_NAME_INVALID;
		}

		/* Can we access this file ? */
		if (!fsp->base_fsp) {
			/* Only do this check on non-stream open. */
			if (file_existed) {
				status = smbd_check_access_rights(conn,
						smb_fname,
						false,
						access_mask);
			} else if (local_flags & O_CREAT){
				status = check_parent_access(conn,
						smb_fname,
						SEC_DIR_ADD_FILE);
			} else {
				/* File didn't exist and no O_CREAT. */
				return NT_STATUS_OBJECT_NAME_NOT_FOUND;
			}
			if (!NT_STATUS_IS_OK(status)) {
				DEBUG(10,("open_file: "
					"%s on file "
					"%s returned %s\n",
					file_existed ?
						"smbd_check_access_rights" :
						"check_parent_access",
					smb_fname_str_dbg(smb_fname),
					nt_errstr(status) ));
				return status;
			}
		}

		/* Actually do the open */
		status = fd_open_atomic(conn, fsp, local_flags,
				unx_mode, p_file_created);
		if (!NT_STATUS_IS_OK(status)) {
			DEBUG(3,("Error opening file %s (%s) (local_flags=%d) "
				 "(flags=%d)\n", smb_fname_str_dbg(smb_fname),
				 nt_errstr(status),local_flags,flags));
			return status;
		}

		ret = SMB_VFS_FSTAT(fsp, &smb_fname->st);
		if (ret == -1) {
			/* If we have an fd, this stat should succeed. */
			DEBUG(0,("Error doing fstat on open file %s "
				"(%s)\n",
				smb_fname_str_dbg(smb_fname),
				strerror(errno) ));
			status = map_nt_error_from_unix(errno);
			fd_close(fsp);
			return status;
		}

		if (*p_file_created) {
			/* We created this file. */

			bool need_re_stat = false;
			/* Do all inheritance work after we've
			   done a successful fstat call and filled
			   in the stat struct in fsp->fsp_name. */

			/* Inherit the ACL if required */
			if (lp_inherit_perms(SNUM(conn))) {
				inherit_access_posix_acl(conn, parent_dir,
							 smb_fname->base_name,
							 unx_mode);
				need_re_stat = true;
			}

			/* Change the owner if required. */
			if (lp_inherit_owner(SNUM(conn))) {
				change_file_owner_to_parent(conn, parent_dir,
							    fsp);
				need_re_stat = true;
			}

			if (need_re_stat) {
				ret = SMB_VFS_FSTAT(fsp, &smb_fname->st);
				/* If we have an fd, this stat should succeed. */
				if (ret == -1) {
					DEBUG(0,("Error doing fstat on open file %s "
						 "(%s)\n",
						 smb_fname_str_dbg(smb_fname),
						 strerror(errno) ));
				}
			}

			notify_fname(conn, NOTIFY_ACTION_ADDED,
				     FILE_NOTIFY_CHANGE_FILE_NAME,
				     smb_fname->base_name);
		}
	} else {
		fsp->fh->fd = -1; /* What we used to call a stat open. */
		if (!file_existed) {
			/* File must exist for a stat open. */
			return NT_STATUS_OBJECT_NAME_NOT_FOUND;
		}

		status = smbd_check_access_rights(conn,
				smb_fname,
				false,
				access_mask);

		if (NT_STATUS_EQUAL(status, NT_STATUS_OBJECT_NAME_NOT_FOUND) &&
				fsp->posix_open &&
				S_ISLNK(smb_fname->st.st_ex_mode)) {
			/* This is a POSIX stat open for delete
			 * or rename on a symlink that points
			 * nowhere. Allow. */
			DEBUG(10,("open_file: allowing POSIX "
				  "open on bad symlink %s\n",
				  smb_fname_str_dbg(smb_fname)));
			status = NT_STATUS_OK;
		}

		if (!NT_STATUS_IS_OK(status)) {
			DEBUG(10,("open_file: "
				"smbd_check_access_rights on file "
				"%s returned %s\n",
				smb_fname_str_dbg(smb_fname),
				nt_errstr(status) ));
			return status;
		}
	}

	/*
	 * POSIX allows read-only opens of directories. We don't
	 * want to do this (we use a different code path for this)
	 * so catch a directory open and return an EISDIR. JRA.
	 */

	if(S_ISDIR(smb_fname->st.st_ex_mode)) {
		fd_close(fsp);
		errno = EISDIR;
		return NT_STATUS_FILE_IS_A_DIRECTORY;
	}

	fsp->file_id = vfs_file_id_from_sbuf(conn, &smb_fname->st);
	fsp->vuid = req ? req->vuid : UID_FIELD_INVALID;
	fsp->file_pid = req ? req->smbpid : 0;
	fsp->can_lock = True;
	fsp->can_read = ((access_mask & FILE_READ_DATA) != 0);
	fsp->can_write =
		CAN_WRITE(conn) &&
		((access_mask & (FILE_WRITE_DATA | FILE_APPEND_DATA)) != 0);
	fsp->print_file = NULL;
	fsp->modified = False;
	fsp->sent_oplock_break = NO_BREAK_SENT;
	fsp->is_directory = False;
	if (conn->aio_write_behind_list &&
	    is_in_path(smb_fname->base_name, conn->aio_write_behind_list,
		       conn->case_sensitive)) {
		fsp->aio_write_behind = True;
	}

	fsp->wcp = NULL; /* Write cache pointer. */

	DEBUG(2,("%s opened file %s read=%s write=%s (numopen=%d)\n",
		 conn->session_info->unix_info->unix_name,
		 smb_fname_str_dbg(smb_fname),
		 BOOLSTR(fsp->can_read), BOOLSTR(fsp->can_write),
		 conn->num_files_open));

	errno = 0;
	return NT_STATUS_OK;
}