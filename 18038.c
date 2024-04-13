NTSTATUS change_dir_owner_to_parent(connection_struct *conn,
				       const char *inherit_from_dir,
				       const char *fname,
				       SMB_STRUCT_STAT *psbuf)
{
	struct smb_filename *smb_fname_parent;
	struct smb_filename *smb_fname_cwd = NULL;
	char *saved_dir = NULL;
	TALLOC_CTX *ctx = talloc_tos();
	NTSTATUS status = NT_STATUS_OK;
	int ret;

	smb_fname_parent = synthetic_smb_fname(ctx, inherit_from_dir,
					       NULL, NULL);
	if (smb_fname_parent == NULL) {
		return NT_STATUS_NO_MEMORY;
	}

	ret = SMB_VFS_STAT(conn, smb_fname_parent);
	if (ret == -1) {
		status = map_nt_error_from_unix(errno);
		DEBUG(0,("change_dir_owner_to_parent: failed to stat parent "
			 "directory %s. Error was %s\n",
			 smb_fname_str_dbg(smb_fname_parent),
			 strerror(errno)));
		goto out;
	}

	/* We've already done an lstat into psbuf, and we know it's a
	   directory. If we can cd into the directory and the dev/ino
	   are the same then we can safely chown without races as
	   we're locking the directory in place by being in it.  This
	   should work on any UNIX (thanks tridge :-). JRA.
	*/

	saved_dir = vfs_GetWd(ctx,conn);
	if (!saved_dir) {
		status = map_nt_error_from_unix(errno);
		DEBUG(0,("change_dir_owner_to_parent: failed to get "
			 "current working directory. Error was %s\n",
			 strerror(errno)));
		goto out;
	}

	/* Chdir into the new path. */
	if (vfs_ChDir(conn, fname) == -1) {
		status = map_nt_error_from_unix(errno);
		DEBUG(0,("change_dir_owner_to_parent: failed to change "
			 "current working directory to %s. Error "
			 "was %s\n", fname, strerror(errno) ));
		goto chdir;
	}

	smb_fname_cwd = synthetic_smb_fname(ctx, ".", NULL, NULL);
	if (smb_fname_cwd == NULL) {
		status = NT_STATUS_NO_MEMORY;
		goto chdir;
	}

	ret = SMB_VFS_STAT(conn, smb_fname_cwd);
	if (ret == -1) {
		status = map_nt_error_from_unix(errno);
		DEBUG(0,("change_dir_owner_to_parent: failed to stat "
			 "directory '.' (%s) Error was %s\n",
			 fname, strerror(errno)));
		goto chdir;
	}

	/* Ensure we're pointing at the same place. */
	if (smb_fname_cwd->st.st_ex_dev != psbuf->st_ex_dev ||
	    smb_fname_cwd->st.st_ex_ino != psbuf->st_ex_ino) {
		DEBUG(0,("change_dir_owner_to_parent: "
			 "device/inode on directory %s changed. "
			 "Refusing to chown !\n", fname ));
		status = NT_STATUS_ACCESS_DENIED;
		goto chdir;
	}

	if (smb_fname_parent->st.st_ex_uid == smb_fname_cwd->st.st_ex_uid) {
		/* Already this uid - no need to change. */
		DEBUG(10,("change_dir_owner_to_parent: directory %s "
			"is already owned by uid %d\n",
			fname,
			(int)smb_fname_cwd->st.st_ex_uid ));
		status = NT_STATUS_OK;
		goto chdir;
	}

	become_root();
	ret = SMB_VFS_LCHOWN(conn, ".", smb_fname_parent->st.st_ex_uid,
			    (gid_t)-1);
	unbecome_root();
	if (ret == -1) {
		status = map_nt_error_from_unix(errno);
		DEBUG(10,("change_dir_owner_to_parent: failed to chown "
			  "directory %s to parent directory uid %u. "
			  "Error was %s\n", fname,
			  (unsigned int)smb_fname_parent->st.st_ex_uid,
			  strerror(errno) ));
	} else {
		DEBUG(10,("change_dir_owner_to_parent: changed ownership of new "
			"directory %s to parent directory uid %u.\n",
			fname, (unsigned int)smb_fname_parent->st.st_ex_uid ));
		/* Ensure the uid entry is updated. */
		psbuf->st_ex_uid = smb_fname_parent->st.st_ex_uid;
	}

 chdir:
	vfs_ChDir(conn,saved_dir);
 out:
	TALLOC_FREE(smb_fname_parent);
	TALLOC_FREE(smb_fname_cwd);
	return status;
}