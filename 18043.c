static NTSTATUS open_file_ntcreate(connection_struct *conn,
			    struct smb_request *req,
			    uint32 access_mask,		/* access bits (FILE_READ_DATA etc.) */
			    uint32 share_access,	/* share constants (FILE_SHARE_READ etc) */
			    uint32 create_disposition,	/* FILE_OPEN_IF etc. */
			    uint32 create_options,	/* options such as delete on close. */
			    uint32 new_dos_attributes,	/* attributes used for new file. */
			    int oplock_request, 	/* internal Samba oplock codes. */
				 			/* Information (FILE_EXISTS etc.) */
			    uint32_t private_flags,     /* Samba specific flags. */
			    int *pinfo,
			    files_struct *fsp)
{
	struct smb_filename *smb_fname = fsp->fsp_name;
	int flags=0;
	int flags2=0;
	bool file_existed = VALID_STAT(smb_fname->st);
	bool def_acl = False;
	bool posix_open = False;
	bool new_file_created = False;
	bool first_open_attempt = true;
	NTSTATUS fsp_open = NT_STATUS_ACCESS_DENIED;
	mode_t new_unx_mode = (mode_t)0;
	mode_t unx_mode = (mode_t)0;
	int info;
	uint32 existing_dos_attributes = 0;
	struct timeval request_time = timeval_zero();
	struct share_mode_lock *lck = NULL;
	uint32 open_access_mask = access_mask;
	NTSTATUS status;
	char *parent_dir;
	SMB_STRUCT_STAT saved_stat = smb_fname->st;
	struct timespec old_write_time;
	struct file_id id;

	if (conn->printer) {
		/*
		 * Printers are handled completely differently.
		 * Most of the passed parameters are ignored.
		 */

		if (pinfo) {
			*pinfo = FILE_WAS_CREATED;
		}

		DEBUG(10, ("open_file_ntcreate: printer open fname=%s\n",
			   smb_fname_str_dbg(smb_fname)));

		if (!req) {
			DEBUG(0,("open_file_ntcreate: printer open without "
				"an SMB request!\n"));
			return NT_STATUS_INTERNAL_ERROR;
		}

		return print_spool_open(fsp, smb_fname->base_name,
					req->vuid);
	}

	if (!parent_dirname(talloc_tos(), smb_fname->base_name, &parent_dir,
			    NULL)) {
		return NT_STATUS_NO_MEMORY;
	}

	if (new_dos_attributes & FILE_FLAG_POSIX_SEMANTICS) {
		posix_open = True;
		unx_mode = (mode_t)(new_dos_attributes & ~FILE_FLAG_POSIX_SEMANTICS);
		new_dos_attributes = 0;
	} else {
		/* Windows allows a new file to be created and
		   silently removes a FILE_ATTRIBUTE_DIRECTORY
		   sent by the client. Do the same. */

		new_dos_attributes &= ~FILE_ATTRIBUTE_DIRECTORY;

		/* We add FILE_ATTRIBUTE_ARCHIVE to this as this mode is only used if the file is
		 * created new. */
		unx_mode = unix_mode(conn, new_dos_attributes | FILE_ATTRIBUTE_ARCHIVE,
				     smb_fname, parent_dir);
	}

	DEBUG(10, ("open_file_ntcreate: fname=%s, dos_attrs=0x%x "
		   "access_mask=0x%x share_access=0x%x "
		   "create_disposition = 0x%x create_options=0x%x "
		   "unix mode=0%o oplock_request=%d private_flags = 0x%x\n",
		   smb_fname_str_dbg(smb_fname), new_dos_attributes,
		   access_mask, share_access, create_disposition,
		   create_options, (unsigned int)unx_mode, oplock_request,
		   (unsigned int)private_flags));

	if ((req == NULL) && ((oplock_request & INTERNAL_OPEN_ONLY) == 0)) {
		DEBUG(0, ("No smb request but not an internal only open!\n"));
		return NT_STATUS_INTERNAL_ERROR;
	}

	/*
	 * Only non-internal opens can be deferred at all
	 */

	if (req) {
		void *ptr;
		if (get_deferred_open_message_state(req,
				&request_time,
				&ptr)) {
			/* Remember the absolute time of the original
			   request with this mid. We'll use it later to
			   see if this has timed out. */

			/* If it was an async create retry, the file
			   didn't exist. */

			if (is_deferred_open_async(ptr)) {
				SET_STAT_INVALID(smb_fname->st);
				file_existed = false;
			}

			/* Ensure we don't reprocess this message. */
			remove_deferred_open_message_smb(req->sconn, req->mid);

			first_open_attempt = false;
		}
	}

	if (!posix_open) {
		new_dos_attributes &= SAMBA_ATTRIBUTES_MASK;
		if (file_existed) {
			existing_dos_attributes = dos_mode(conn, smb_fname);
		}
	}

	/* ignore any oplock requests if oplocks are disabled */
	if (!lp_oplocks(SNUM(conn)) ||
	    IS_VETO_OPLOCK_PATH(conn, smb_fname->base_name)) {
		/* Mask off everything except the private Samba bits. */
		oplock_request &= SAMBA_PRIVATE_OPLOCK_MASK;
	}

	/* this is for OS/2 long file names - say we don't support them */
	if (!lp_posix_pathnames() && strstr(smb_fname->base_name,".+,;=[].")) {
		/* OS/2 Workplace shell fix may be main code stream in a later
		 * release. */
		DEBUG(5,("open_file_ntcreate: OS/2 long filenames are not "
			 "supported.\n"));
		if (use_nt_status()) {
			return NT_STATUS_OBJECT_NAME_NOT_FOUND;
		}
		return NT_STATUS_DOS(ERRDOS, ERRcannotopen);
	}

	switch( create_disposition ) {
		case FILE_OPEN:
			/* If file exists open. If file doesn't exist error. */
			if (!file_existed) {
				DEBUG(5,("open_file_ntcreate: FILE_OPEN "
					 "requested for file %s and file "
					 "doesn't exist.\n",
					 smb_fname_str_dbg(smb_fname)));
				errno = ENOENT;
				return NT_STATUS_OBJECT_NAME_NOT_FOUND;
			}
			break;

		case FILE_OVERWRITE:
			/* If file exists overwrite. If file doesn't exist
			 * error. */
			if (!file_existed) {
				DEBUG(5,("open_file_ntcreate: FILE_OVERWRITE "
					 "requested for file %s and file "
					 "doesn't exist.\n",
					 smb_fname_str_dbg(smb_fname) ));
				errno = ENOENT;
				return NT_STATUS_OBJECT_NAME_NOT_FOUND;
			}
			break;

		case FILE_CREATE:
			/* If file exists error. If file doesn't exist
			 * create. */
			if (file_existed) {
				DEBUG(5,("open_file_ntcreate: FILE_CREATE "
					 "requested for file %s and file "
					 "already exists.\n",
					 smb_fname_str_dbg(smb_fname)));
				if (S_ISDIR(smb_fname->st.st_ex_mode)) {
					errno = EISDIR;
				} else {
					errno = EEXIST;
				}
				return map_nt_error_from_unix(errno);
			}
			break;

		case FILE_SUPERSEDE:
		case FILE_OVERWRITE_IF:
		case FILE_OPEN_IF:
			break;
		default:
			return NT_STATUS_INVALID_PARAMETER;
	}

	flags2 = disposition_to_open_flags(create_disposition);

	/* We only care about matching attributes on file exists and
	 * overwrite. */

	if (!posix_open && file_existed &&
	    ((create_disposition == FILE_OVERWRITE) ||
	     (create_disposition == FILE_OVERWRITE_IF))) {
		if (!open_match_attributes(conn, existing_dos_attributes,
					   new_dos_attributes,
					   smb_fname->st.st_ex_mode,
					   unx_mode, &new_unx_mode)) {
			DEBUG(5,("open_file_ntcreate: attributes missmatch "
				 "for file %s (%x %x) (0%o, 0%o)\n",
				 smb_fname_str_dbg(smb_fname),
				 existing_dos_attributes,
				 new_dos_attributes,
				 (unsigned int)smb_fname->st.st_ex_mode,
				 (unsigned int)unx_mode ));
			errno = EACCES;
			return NT_STATUS_ACCESS_DENIED;
		}
	}

	status = smbd_calculate_access_mask(conn, smb_fname,
					false,
					access_mask,
					&access_mask); 
	if (!NT_STATUS_IS_OK(status)) {
		DEBUG(10, ("open_file_ntcreate: smbd_calculate_access_mask "
			"on file %s returned %s\n",
			smb_fname_str_dbg(smb_fname), nt_errstr(status)));
		return status;
	}

	open_access_mask = access_mask;

	if (flags2 & O_TRUNC) {
		open_access_mask |= FILE_WRITE_DATA; /* This will cause oplock breaks. */
	}

	DEBUG(10, ("open_file_ntcreate: fname=%s, after mapping "
		   "access_mask=0x%x\n", smb_fname_str_dbg(smb_fname),
		    access_mask));

	/*
	 * Note that we ignore the append flag as append does not
	 * mean the same thing under DOS and Unix.
	 */

	flags = calculate_open_access_flags(access_mask, oplock_request,
					    private_flags);

	/*
	 * Currently we only look at FILE_WRITE_THROUGH for create options.
	 */

#if defined(O_SYNC)
	if ((create_options & FILE_WRITE_THROUGH) && lp_strict_sync(SNUM(conn))) {
		flags2 |= O_SYNC;
	}
#endif /* O_SYNC */

	if (posix_open && (access_mask & FILE_APPEND_DATA)) {
		flags2 |= O_APPEND;
	}

	if (!posix_open && !CAN_WRITE(conn)) {
		/*
		 * We should really return a permission denied error if either
		 * O_CREAT or O_TRUNC are set, but for compatibility with
		 * older versions of Samba we just AND them out.
		 */
		flags2 &= ~(O_CREAT|O_TRUNC);
	}

	if (first_open_attempt && lp_kernel_oplocks(SNUM(conn))) {
		/*
		 * With kernel oplocks the open breaking an oplock
		 * blocks until the oplock holder has given up the
		 * oplock or closed the file. We prevent this by first
		 * trying to open the file with O_NONBLOCK (see "man
		 * fcntl" on Linux). For the second try, triggered by
		 * an oplock break response, we do not need this
		 * anymore.
		 *
		 * This is true under the assumption that only Samba
		 * requests kernel oplocks. Once someone else like
		 * NFSv4 starts to use that API, we will have to
		 * modify this by communicating with the NFSv4 server.
		 */
		flags2 |= O_NONBLOCK;
	}

	/*
	 * Ensure we can't write on a read-only share or file.
	 */

	if (flags != O_RDONLY && file_existed &&
	    (!CAN_WRITE(conn) || IS_DOS_READONLY(existing_dos_attributes))) {
		DEBUG(5,("open_file_ntcreate: write access requested for "
			 "file %s on read only %s\n",
			 smb_fname_str_dbg(smb_fname),
			 !CAN_WRITE(conn) ? "share" : "file" ));
		errno = EACCES;
		return NT_STATUS_ACCESS_DENIED;
	}

	fsp->file_id = vfs_file_id_from_sbuf(conn, &smb_fname->st);
	fsp->share_access = share_access;
	fsp->fh->private_options = private_flags;
	fsp->access_mask = open_access_mask; /* We change this to the
					      * requested access_mask after
					      * the open is done. */
	fsp->posix_open = posix_open;

	/* Ensure no SAMBA_PRIVATE bits can be set. */
	fsp->oplock_type = (oplock_request & ~SAMBA_PRIVATE_OPLOCK_MASK);

	if (timeval_is_zero(&request_time)) {
		request_time = fsp->open_time;
	}

	/*
	 * Ensure we pay attention to default ACLs on directories if required.
	 */

        if ((flags2 & O_CREAT) && lp_inherit_acls(SNUM(conn)) &&
	    (def_acl = directory_has_default_acl(conn, parent_dir))) {
		unx_mode = (0777 & lp_create_mask(SNUM(conn)));
	}

	DEBUG(4,("calling open_file with flags=0x%X flags2=0x%X mode=0%o, "
		"access_mask = 0x%x, open_access_mask = 0x%x\n",
		 (unsigned int)flags, (unsigned int)flags2,
		 (unsigned int)unx_mode, (unsigned int)access_mask,
		 (unsigned int)open_access_mask));

	fsp_open = open_file(fsp, conn, req, parent_dir,
			     flags|flags2, unx_mode, access_mask,
			     open_access_mask, &new_file_created);

	if (NT_STATUS_EQUAL(fsp_open, NT_STATUS_NETWORK_BUSY)) {
		struct deferred_open_record state;

		/*
		 * EWOULDBLOCK/EAGAIN maps to NETWORK_BUSY.
		 */
		if (file_existed && S_ISFIFO(fsp->fsp_name->st.st_ex_mode)) {
			DEBUG(10, ("FIFO busy\n"));
			return NT_STATUS_NETWORK_BUSY;
		}
		if (req == NULL) {
			DEBUG(10, ("Internal open busy\n"));
			return NT_STATUS_NETWORK_BUSY;
		}

		/*
		 * From here on we assume this is an oplock break triggered
		 */

		lck = get_existing_share_mode_lock(talloc_tos(), fsp->file_id);
		if (lck == NULL) {
			state.delayed_for_oplocks = false;
			state.async_open = false;
			state.id = fsp->file_id;
			defer_open(NULL, request_time, timeval_set(0, 0),
				   req, &state);
			DEBUG(10, ("No share mode lock found after "
				   "EWOULDBLOCK, retrying sync\n"));
			return NT_STATUS_SHARING_VIOLATION;
		}

		if (!validate_oplock_types(lck)) {
			smb_panic("validate_oplock_types failed");
		}

		if (delay_for_oplock(fsp, 0, lck, false, create_disposition)) {
			schedule_defer_open(lck, request_time, req);
			TALLOC_FREE(lck);
			DEBUG(10, ("Sent oplock break request to kernel "
				   "oplock holder\n"));
			return NT_STATUS_SHARING_VIOLATION;
		}

		/*
		 * No oplock from Samba around. Immediately retry with
		 * a blocking open.
		 */
		state.delayed_for_oplocks = false;
		state.async_open = false;
		state.id = lck->data->id;
		defer_open(lck, request_time, timeval_set(0, 0), req, &state);
		TALLOC_FREE(lck);
		DEBUG(10, ("No Samba oplock around after EWOULDBLOCK. "
			   "Retrying sync\n"));
		return NT_STATUS_SHARING_VIOLATION;
	}

	if (!NT_STATUS_IS_OK(fsp_open)) {
		if (NT_STATUS_EQUAL(fsp_open, NT_STATUS_RETRY)) {
			schedule_async_open(request_time, req);
		}
		return fsp_open;
	}

	if (file_existed && !check_same_dev_ino(&saved_stat, &smb_fname->st)) {
		/*
		 * The file did exist, but some other (local or NFS)
		 * process either renamed/unlinked and re-created the
		 * file with different dev/ino after we walked the path,
		 * but before we did the open. We could retry the
		 * open but it's a rare enough case it's easier to
		 * just fail the open to prevent creating any problems
		 * in the open file db having the wrong dev/ino key.
		 */
		fd_close(fsp);
		DEBUG(1,("open_file_ntcreate: file %s - dev/ino mismatch. "
			"Old (dev=0x%llu, ino =0x%llu). "
			"New (dev=0x%llu, ino=0x%llu). Failing open "
			" with NT_STATUS_ACCESS_DENIED.\n",
			 smb_fname_str_dbg(smb_fname),
			 (unsigned long long)saved_stat.st_ex_dev,
			 (unsigned long long)saved_stat.st_ex_ino,
			 (unsigned long long)smb_fname->st.st_ex_dev,
			 (unsigned long long)smb_fname->st.st_ex_ino));
		return NT_STATUS_ACCESS_DENIED;
	}

	old_write_time = smb_fname->st.st_ex_mtime;

	/*
	 * Deal with the race condition where two smbd's detect the
	 * file doesn't exist and do the create at the same time. One
	 * of them will win and set a share mode, the other (ie. this
	 * one) should check if the requested share mode for this
	 * create is allowed.
	 */

	/*
	 * Now the file exists and fsp is successfully opened,
	 * fsp->dev and fsp->inode are valid and should replace the
	 * dev=0,inode=0 from a non existent file. Spotted by
	 * Nadav Danieli <nadavd@exanet.com>. JRA.
	 */

	id = fsp->file_id;

	lck = get_share_mode_lock(talloc_tos(), id,
				  conn->connectpath,
				  smb_fname, &old_write_time);

	if (lck == NULL) {
		DEBUG(0, ("open_file_ntcreate: Could not get share "
			  "mode lock for %s\n",
			  smb_fname_str_dbg(smb_fname)));
		fd_close(fsp);
		return NT_STATUS_SHARING_VIOLATION;
	}

	/* Get the types we need to examine. */
	if (!validate_oplock_types(lck)) {
		smb_panic("validate_oplock_types failed");
	}

	if (has_delete_on_close(lck, fsp->name_hash)) {
		TALLOC_FREE(lck);
		fd_close(fsp);
		return NT_STATUS_DELETE_PENDING;
	}

	status = open_mode_check(conn, lck,
				 access_mask, share_access);

	if (NT_STATUS_EQUAL(status, NT_STATUS_SHARING_VIOLATION) ||
	    (lck->data->num_share_modes > 0)) {
		/*
		 * This comes from ancient times out of open_mode_check. I
		 * have no clue whether this is still necessary. I can't think
		 * of a case where this would actually matter further down in
		 * this function. I leave it here for further investigation
		 * :-)
		 */
		file_existed = true;
	}

	if ((req != NULL) &&
	    delay_for_oplock(
		    fsp, oplock_request, lck,
		    NT_STATUS_EQUAL(status, NT_STATUS_SHARING_VIOLATION),
		    create_disposition)) {
		schedule_defer_open(lck, request_time, req);
		TALLOC_FREE(lck);
		fd_close(fsp);
		return NT_STATUS_SHARING_VIOLATION;
	}

	if (!NT_STATUS_IS_OK(status)) {
		uint32 can_access_mask;
		bool can_access = True;

		SMB_ASSERT(NT_STATUS_EQUAL(status, NT_STATUS_SHARING_VIOLATION));

		/* Check if this can be done with the deny_dos and fcb
		 * calls. */
		if (private_flags &
		    (NTCREATEX_OPTIONS_PRIVATE_DENY_DOS|
		     NTCREATEX_OPTIONS_PRIVATE_DENY_FCB)) {
			if (req == NULL) {
				DEBUG(0, ("DOS open without an SMB "
					  "request!\n"));
				TALLOC_FREE(lck);
				fd_close(fsp);
				return NT_STATUS_INTERNAL_ERROR;
			}

			/* Use the client requested access mask here,
			 * not the one we open with. */
			status = fcb_or_dos_open(req,
						 conn,
						 fsp,
						 smb_fname,
						 id,
						 req->smbpid,
						 req->vuid,
						 access_mask,
						 share_access,
						 create_options);

			if (NT_STATUS_IS_OK(status)) {
				TALLOC_FREE(lck);
				if (pinfo) {
					*pinfo = FILE_WAS_OPENED;
				}
				return NT_STATUS_OK;
			}
		}

		/*
		 * This next line is a subtlety we need for
		 * MS-Access. If a file open will fail due to share
		 * permissions and also for security (access) reasons,
		 * we need to return the access failed error, not the
		 * share error. We can't open the file due to kernel
		 * oplock deadlock (it's possible we failed above on
		 * the open_mode_check()) so use a userspace check.
		 */

		if (flags & O_RDWR) {
			can_access_mask = FILE_READ_DATA|FILE_WRITE_DATA;
		} else if (flags & O_WRONLY) {
			can_access_mask = FILE_WRITE_DATA;
		} else {
			can_access_mask = FILE_READ_DATA;
		}

		if (((can_access_mask & FILE_WRITE_DATA) &&
		     !CAN_WRITE(conn)) ||
		    !NT_STATUS_IS_OK(smbd_check_access_rights(conn,
							      smb_fname,
							      false,
							      can_access_mask))) {
			can_access = False;
		}

		/*
		 * If we're returning a share violation, ensure we
		 * cope with the braindead 1 second delay (SMB1 only).
		 */

		if (!(oplock_request & INTERNAL_OPEN_ONLY) &&
		    !conn->sconn->using_smb2 &&
		    lp_defer_sharing_violations()) {
			struct timeval timeout;
			struct deferred_open_record state;
			int timeout_usecs;

			/* this is a hack to speed up torture tests
			   in 'make test' */
			timeout_usecs = lp_parm_int(SNUM(conn),
						    "smbd","sharedelay",
						    SHARING_VIOLATION_USEC_WAIT);

			/* This is a relative time, added to the absolute
			   request_time value to get the absolute timeout time.
			   Note that if this is the second or greater time we enter
			   this codepath for this particular request mid then
			   request_time is left as the absolute time of the *first*
			   time this request mid was processed. This is what allows
			   the request to eventually time out. */

			timeout = timeval_set(0, timeout_usecs);

			/* Nothing actually uses state.delayed_for_oplocks
			   but it's handy to differentiate in debug messages
			   between a 30 second delay due to oplock break, and
			   a 1 second delay for share mode conflicts. */

			state.delayed_for_oplocks = False;
			state.async_open = false;
			state.id = id;

			if ((req != NULL)
			    && !request_timed_out(request_time,
						  timeout)) {
				defer_open(lck, request_time, timeout,
					   req, &state);
			}
		}

		TALLOC_FREE(lck);
		fd_close(fsp);
		if (can_access) {
			/*
			 * We have detected a sharing violation here
			 * so return the correct error code
			 */
			status = NT_STATUS_SHARING_VIOLATION;
		} else {
			status = NT_STATUS_ACCESS_DENIED;
		}
		return status;
	}

	grant_fsp_oplock_type(fsp, lck, oplock_request);

	/*
	 * We have the share entry *locked*.....
	 */

	/* Delete streams if create_disposition requires it */
	if (!new_file_created && clear_ads(create_disposition) &&
	    !is_ntfs_stream_smb_fname(smb_fname)) {
		status = delete_all_streams(conn, smb_fname->base_name);
		if (!NT_STATUS_IS_OK(status)) {
			TALLOC_FREE(lck);
			fd_close(fsp);
			return status;
		}
	}

	/* note that we ignore failure for the following. It is
           basically a hack for NFS, and NFS will never set one of
           these only read them. Nobody but Samba can ever set a deny
           mode and we have already checked our more authoritative
           locking database for permission to set this deny mode. If
           the kernel refuses the operations then the kernel is wrong.
	   note that GPFS supports it as well - jmcd */

	if (fsp->fh->fd != -1 && lp_kernel_share_modes(SNUM(conn))) {
		int ret_flock;
		ret_flock = SMB_VFS_KERNEL_FLOCK(fsp, share_access, access_mask);
		if(ret_flock == -1 ){

			TALLOC_FREE(lck);
			fd_close(fsp);

			return NT_STATUS_SHARING_VIOLATION;
		}
	}

	/*
	 * At this point onwards, we can guarantee that the share entry
	 * is locked, whether we created the file or not, and that the
	 * deny mode is compatible with all current opens.
	 */

	/*
	 * According to Samba4, SEC_FILE_READ_ATTRIBUTE is always granted,
	 * but we don't have to store this - just ignore it on access check.
	 */
	if (conn->sconn->using_smb2) {
		/*
		 * SMB2 doesn't return it (according to Microsoft tests).
		 * Test Case: TestSuite_ScenarioNo009GrantedAccessTestS0
		 * File created with access = 0x7 (Read, Write, Delete)
		 * Query Info on file returns 0x87 (Read, Write, Delete, Read Attributes)
		 */
		fsp->access_mask = access_mask;
	} else {
		/* But SMB1 does. */
		fsp->access_mask = access_mask | FILE_READ_ATTRIBUTES;
	}

	if (file_existed) {
		/* stat opens on existing files don't get oplocks. */
		if (is_stat_open(open_access_mask)) {
			fsp->oplock_type = NO_OPLOCK;
		}
	}

	if (new_file_created) {
		info = FILE_WAS_CREATED;
	} else {
		if (flags2 & O_TRUNC) {
			info = FILE_WAS_OVERWRITTEN;
		} else {
			info = FILE_WAS_OPENED;
		}
	}

	if (pinfo) {
		*pinfo = info;
	}

	/*
	 * Setup the oplock info in both the shared memory and
	 * file structs.
	 */

	status = set_file_oplock(fsp);
	if (!NT_STATUS_IS_OK(status)) {
		/*
		 * Could not get the kernel oplock
		 */
		fsp->oplock_type = NO_OPLOCK;
	}

	if (!set_share_mode(lck, fsp, get_current_uid(conn),
			    req ? req->mid : 0,
			    fsp->oplock_type)) {
		TALLOC_FREE(lck);
		fd_close(fsp);
		return NT_STATUS_NO_MEMORY;
	}

	/* Handle strange delete on close create semantics. */
	if (create_options & FILE_DELETE_ON_CLOSE) {

		status = can_set_delete_on_close(fsp, new_dos_attributes);

		if (!NT_STATUS_IS_OK(status)) {
			/* Remember to delete the mode we just added. */
			del_share_mode(lck, fsp);
			TALLOC_FREE(lck);
			fd_close(fsp);
			return status;
		}
		/* Note that here we set the *inital* delete on close flag,
		   not the regular one. The magic gets handled in close. */
		fsp->initial_delete_on_close = True;
	}

	if (info != FILE_WAS_OPENED) {
		/* Files should be initially set as archive */
		if (lp_map_archive(SNUM(conn)) ||
		    lp_store_dos_attributes(SNUM(conn))) {
			if (!posix_open) {
				if (file_set_dosmode(conn, smb_fname,
					    new_dos_attributes | FILE_ATTRIBUTE_ARCHIVE,
					    parent_dir, true) == 0) {
					unx_mode = smb_fname->st.st_ex_mode;
				}
			}
		}
	}

	/* Determine sparse flag. */
	if (posix_open) {
		/* POSIX opens are sparse by default. */
		fsp->is_sparse = true;
	} else {
		fsp->is_sparse = (file_existed &&
			(existing_dos_attributes & FILE_ATTRIBUTE_SPARSE));
	}

	/*
	 * Take care of inherited ACLs on created files - if default ACL not
	 * selected.
	 */

	if (!posix_open && new_file_created && !def_acl) {

		int saved_errno = errno; /* We might get ENOSYS in the next
					  * call.. */

		if (SMB_VFS_FCHMOD_ACL(fsp, unx_mode) == -1 &&
		    errno == ENOSYS) {
			errno = saved_errno; /* Ignore ENOSYS */
		}

	} else if (new_unx_mode) {

		int ret = -1;

		/* Attributes need changing. File already existed. */

		{
			int saved_errno = errno; /* We might get ENOSYS in the
						  * next call.. */
			ret = SMB_VFS_FCHMOD_ACL(fsp, new_unx_mode);

			if (ret == -1 && errno == ENOSYS) {
				errno = saved_errno; /* Ignore ENOSYS */
			} else {
				DEBUG(5, ("open_file_ntcreate: reset "
					  "attributes of file %s to 0%o\n",
					  smb_fname_str_dbg(smb_fname),
					  (unsigned int)new_unx_mode));
				ret = 0; /* Don't do the fchmod below. */
			}
		}

		if ((ret == -1) &&
		    (SMB_VFS_FCHMOD(fsp, new_unx_mode) == -1))
			DEBUG(5, ("open_file_ntcreate: failed to reset "
				  "attributes of file %s to 0%o\n",
				  smb_fname_str_dbg(smb_fname),
				  (unsigned int)new_unx_mode));
	}

	TALLOC_FREE(lck);

	return NT_STATUS_OK;
}