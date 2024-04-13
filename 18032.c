void change_file_owner_to_parent(connection_struct *conn,
					const char *inherit_from_dir,
					files_struct *fsp)
{
	struct smb_filename *smb_fname_parent;
	int ret;

	smb_fname_parent = synthetic_smb_fname(talloc_tos(), inherit_from_dir,
					       NULL, NULL);
	if (smb_fname_parent == NULL) {
		return;
	}

	ret = SMB_VFS_STAT(conn, smb_fname_parent);
	if (ret == -1) {
		DEBUG(0,("change_file_owner_to_parent: failed to stat parent "
			 "directory %s. Error was %s\n",
			 smb_fname_str_dbg(smb_fname_parent),
			 strerror(errno)));
		TALLOC_FREE(smb_fname_parent);
		return;
	}

	if (smb_fname_parent->st.st_ex_uid == fsp->fsp_name->st.st_ex_uid) {
		/* Already this uid - no need to change. */
		DEBUG(10,("change_file_owner_to_parent: file %s "
			"is already owned by uid %d\n",
			fsp_str_dbg(fsp),
			(int)fsp->fsp_name->st.st_ex_uid ));
		TALLOC_FREE(smb_fname_parent);
		return;
	}

	become_root();
	ret = SMB_VFS_FCHOWN(fsp, smb_fname_parent->st.st_ex_uid, (gid_t)-1);
	unbecome_root();
	if (ret == -1) {
		DEBUG(0,("change_file_owner_to_parent: failed to fchown "
			 "file %s to parent directory uid %u. Error "
			 "was %s\n", fsp_str_dbg(fsp),
			 (unsigned int)smb_fname_parent->st.st_ex_uid,
			 strerror(errno) ));
	} else {
		DEBUG(10,("change_file_owner_to_parent: changed new file %s to "
			"parent directory uid %u.\n", fsp_str_dbg(fsp),
			(unsigned int)smb_fname_parent->st.st_ex_uid));
		/* Ensure the uid entry is updated. */
		fsp->fsp_name->st.st_ex_uid = smb_fname_parent->st.st_ex_uid;
	}

	TALLOC_FREE(smb_fname_parent);
}