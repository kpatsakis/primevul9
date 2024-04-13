static NTSTATUS mkdir_internal(connection_struct *conn,
			       struct smb_filename *smb_dname,
			       uint32 file_attributes)
{
	mode_t mode;
	char *parent_dir = NULL;
	NTSTATUS status;
	bool posix_open = false;
	bool need_re_stat = false;
	uint32_t access_mask = SEC_DIR_ADD_SUBDIR;

	if (!CAN_WRITE(conn) || (access_mask & ~(conn->share_access))) {
		DEBUG(5,("mkdir_internal: failing share access "
			 "%s\n", lp_servicename(talloc_tos(), SNUM(conn))));
		return NT_STATUS_ACCESS_DENIED;
	}

	if (!parent_dirname(talloc_tos(), smb_dname->base_name, &parent_dir,
			    NULL)) {
		return NT_STATUS_NO_MEMORY;
	}

	if (file_attributes & FILE_FLAG_POSIX_SEMANTICS) {
		posix_open = true;
		mode = (mode_t)(file_attributes & ~FILE_FLAG_POSIX_SEMANTICS);
	} else {
		mode = unix_mode(conn, FILE_ATTRIBUTE_DIRECTORY, smb_dname, parent_dir);
	}

	status = check_parent_access(conn,
					smb_dname,
					access_mask);
	if(!NT_STATUS_IS_OK(status)) {
		DEBUG(5,("mkdir_internal: check_parent_access "
			"on directory %s for path %s returned %s\n",
			parent_dir,
			smb_dname->base_name,
			nt_errstr(status) ));
		return status;
	}

	if (SMB_VFS_MKDIR(conn, smb_dname->base_name, mode) != 0) {
		return map_nt_error_from_unix(errno);
	}

	/* Ensure we're checking for a symlink here.... */
	/* We don't want to get caught by a symlink racer. */

	if (SMB_VFS_LSTAT(conn, smb_dname) == -1) {
		DEBUG(2, ("Could not stat directory '%s' just created: %s\n",
			  smb_fname_str_dbg(smb_dname), strerror(errno)));
		return map_nt_error_from_unix(errno);
	}

	if (!S_ISDIR(smb_dname->st.st_ex_mode)) {
		DEBUG(0, ("Directory '%s' just created is not a directory !\n",
			  smb_fname_str_dbg(smb_dname)));
		return NT_STATUS_NOT_A_DIRECTORY;
	}

	if (lp_store_dos_attributes(SNUM(conn))) {
		if (!posix_open) {
			file_set_dosmode(conn, smb_dname,
					 file_attributes | FILE_ATTRIBUTE_DIRECTORY,
					 parent_dir, true);
		}
	}

	if (lp_inherit_perms(SNUM(conn))) {
		inherit_access_posix_acl(conn, parent_dir,
					 smb_dname->base_name, mode);
		need_re_stat = true;
	}

	if (!posix_open) {
		/*
		 * Check if high bits should have been set,
		 * then (if bits are missing): add them.
		 * Consider bits automagically set by UNIX, i.e. SGID bit from parent
		 * dir.
		 */
		if ((mode & ~(S_IRWXU|S_IRWXG|S_IRWXO)) &&
		    (mode & ~smb_dname->st.st_ex_mode)) {
			SMB_VFS_CHMOD(conn, smb_dname->base_name,
				      (smb_dname->st.st_ex_mode |
					  (mode & ~smb_dname->st.st_ex_mode)));
			need_re_stat = true;
		}
	}

	/* Change the owner if required. */
	if (lp_inherit_owner(SNUM(conn))) {
		change_dir_owner_to_parent(conn, parent_dir,
					   smb_dname->base_name,
					   &smb_dname->st);
		need_re_stat = true;
	}

	if (need_re_stat) {
		if (SMB_VFS_LSTAT(conn, smb_dname) == -1) {
			DEBUG(2, ("Could not stat directory '%s' just created: %s\n",
			  smb_fname_str_dbg(smb_dname), strerror(errno)));
			return map_nt_error_from_unix(errno);
		}
	}

	notify_fname(conn, NOTIFY_ACTION_ADDED, FILE_NOTIFY_CHANGE_DIR_NAME,
		     smb_dname->base_name);

	return NT_STATUS_OK;
}