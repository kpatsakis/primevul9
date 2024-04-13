static NTSTATUS check_parent_access(struct connection_struct *conn,
				struct smb_filename *smb_fname,
				uint32_t access_mask)
{
	NTSTATUS status;
	char *parent_dir = NULL;
	struct security_descriptor *parent_sd = NULL;
	uint32_t access_granted = 0;

	if (!parent_dirname(talloc_tos(),
				smb_fname->base_name,
				&parent_dir,
				NULL)) {
		return NT_STATUS_NO_MEMORY;
	}

	if (get_current_uid(conn) == (uid_t)0) {
		/* I'm sorry sir, I didn't know you were root... */
		DEBUG(10,("check_parent_access: root override "
			"on %s. Granting 0x%x\n",
			smb_fname_str_dbg(smb_fname),
			(unsigned int)access_mask ));
		return NT_STATUS_OK;
	}

	status = SMB_VFS_GET_NT_ACL(conn,
				parent_dir,
				SECINFO_DACL,
				    talloc_tos(),
				&parent_sd);

	if (!NT_STATUS_IS_OK(status)) {
		DEBUG(5,("check_parent_access: SMB_VFS_GET_NT_ACL failed for "
			"%s with error %s\n",
			parent_dir,
			nt_errstr(status)));
		return status;
	}

 	/*
	 * If we can access the path to this file, by
	 * default we have FILE_READ_ATTRIBUTES from the
	 * containing directory. See the section:
	 * "Algorithm to Check Access to an Existing File"
	 * in MS-FSA.pdf.
	 *
	 * se_file_access_check() also takes care of
	 * owner WRITE_DAC and READ_CONTROL.
	 */
	status = se_file_access_check(parent_sd,
				get_current_nttok(conn),
				false,
				(access_mask & ~FILE_READ_ATTRIBUTES),
				&access_granted);
	if(!NT_STATUS_IS_OK(status)) {
		DEBUG(5,("check_parent_access: access check "
			"on directory %s for "
			"path %s for mask 0x%x returned (0x%x) %s\n",
			parent_dir,
			smb_fname->base_name,
			access_mask,
			access_granted,
			nt_errstr(status) ));
		return status;
	}

	return NT_STATUS_OK;
}