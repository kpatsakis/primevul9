static NTSTATUS smbd_calculate_maximum_allowed_access(
	connection_struct *conn,
	const struct smb_filename *smb_fname,
	bool use_privs,
	uint32_t *p_access_mask)
{
	struct security_descriptor *sd;
	uint32_t access_granted;
	NTSTATUS status;

	if (!use_privs && (get_current_uid(conn) == (uid_t)0)) {
		*p_access_mask |= FILE_GENERIC_ALL;
		return NT_STATUS_OK;
	}

	status = SMB_VFS_GET_NT_ACL(conn, smb_fname->base_name,
				    (SECINFO_OWNER |
				     SECINFO_GROUP |
				     SECINFO_DACL),
				    talloc_tos(), &sd);

	if (NT_STATUS_EQUAL(status, NT_STATUS_OBJECT_NAME_NOT_FOUND)) {
		/*
		 * File did not exist
		 */
		*p_access_mask = FILE_GENERIC_ALL;
		return NT_STATUS_OK;
	}
	if (!NT_STATUS_IS_OK(status)) {
		DEBUG(10,("Could not get acl on file %s: %s\n",
			  smb_fname_str_dbg(smb_fname),
			  nt_errstr(status)));
		return NT_STATUS_ACCESS_DENIED;
	}

	/*
	 * If we can access the path to this file, by
	 * default we have FILE_READ_ATTRIBUTES from the
	 * containing directory. See the section:
	 * "Algorithm to Check Access to an Existing File"
	 * in MS-FSA.pdf.
	 *
	 * se_file_access_check()
	 * also takes care of owner WRITE_DAC and READ_CONTROL.
	 */
	status = se_file_access_check(sd,
				 get_current_nttok(conn),
				 use_privs,
				 (*p_access_mask & ~FILE_READ_ATTRIBUTES),
				 &access_granted);

	TALLOC_FREE(sd);

	if (!NT_STATUS_IS_OK(status)) {
		DEBUG(10, ("Access denied on file %s: "
			   "when calculating maximum access\n",
			   smb_fname_str_dbg(smb_fname)));
		return NT_STATUS_ACCESS_DENIED;
	}
	*p_access_mask = (access_granted | FILE_READ_ATTRIBUTES);

	if (!(access_granted & DELETE_ACCESS)) {
		if (can_delete_file_in_directory(conn, smb_fname)) {
			*p_access_mask |= DELETE_ACCESS;
		}
	}

	return NT_STATUS_OK;
}