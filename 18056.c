NTSTATUS smbd_check_access_rights(struct connection_struct *conn,
				const struct smb_filename *smb_fname,
				bool use_privs,
				uint32_t access_mask)
{
	/* Check if we have rights to open. */
	NTSTATUS status;
	struct security_descriptor *sd = NULL;
	uint32_t rejected_share_access;
	uint32_t rejected_mask = access_mask;
	uint32_t do_not_check_mask = 0;

	rejected_share_access = access_mask & ~(conn->share_access);

	if (rejected_share_access) {
		DEBUG(10, ("smbd_check_access_rights: rejected share access 0x%x "
			"on %s (0x%x)\n",
			(unsigned int)access_mask,
			smb_fname_str_dbg(smb_fname),
			(unsigned int)rejected_share_access ));
		return NT_STATUS_ACCESS_DENIED;
	}

	if (!use_privs && get_current_uid(conn) == (uid_t)0) {
		/* I'm sorry sir, I didn't know you were root... */
		DEBUG(10,("smbd_check_access_rights: root override "
			"on %s. Granting 0x%x\n",
			smb_fname_str_dbg(smb_fname),
			(unsigned int)access_mask ));
		return NT_STATUS_OK;
	}

	if ((access_mask & DELETE_ACCESS) && !lp_acl_check_permissions(SNUM(conn))) {
		DEBUG(10,("smbd_check_access_rights: not checking ACL "
			"on DELETE_ACCESS on file %s. Granting 0x%x\n",
			smb_fname_str_dbg(smb_fname),
			(unsigned int)access_mask ));
		return NT_STATUS_OK;
	}

	if (access_mask == DELETE_ACCESS &&
			VALID_STAT(smb_fname->st) &&
			S_ISLNK(smb_fname->st.st_ex_mode)) {
		/* We can always delete a symlink. */
		DEBUG(10,("smbd_check_access_rights: not checking ACL "
			"on DELETE_ACCESS on symlink %s.\n",
			smb_fname_str_dbg(smb_fname) ));
		return NT_STATUS_OK;
	}

	status = SMB_VFS_GET_NT_ACL(conn, smb_fname->base_name,
			(SECINFO_OWNER |
			SECINFO_GROUP |
			 SECINFO_DACL), talloc_tos(), &sd);

	if (!NT_STATUS_IS_OK(status)) {
		DEBUG(10, ("smbd_check_access_rights: Could not get acl "
			"on %s: %s\n",
			smb_fname_str_dbg(smb_fname),
			nt_errstr(status)));

		if (NT_STATUS_EQUAL(status, NT_STATUS_ACCESS_DENIED)) {
			goto access_denied;
		}

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
	do_not_check_mask = FILE_READ_ATTRIBUTES;

	/*
	 * Samba 3.6 and earlier granted execute access even
	 * if the ACL did not contain execute rights.
	 * Samba 4.0 is more correct and checks it.
	 * The compatibilty mode allows to skip this check
	 * to smoothen upgrades.
	 */
	if (lp_acl_allow_execute_always(SNUM(conn))) {
		do_not_check_mask |= FILE_EXECUTE;
	}

	status = se_file_access_check(sd,
				get_current_nttok(conn),
				use_privs,
				(access_mask & ~do_not_check_mask),
				&rejected_mask);

	DEBUG(10,("smbd_check_access_rights: file %s requesting "
		"0x%x returning 0x%x (%s)\n",
		smb_fname_str_dbg(smb_fname),
		(unsigned int)access_mask,
		(unsigned int)rejected_mask,
		nt_errstr(status) ));

	if (!NT_STATUS_IS_OK(status)) {
		if (DEBUGLEVEL >= 10) {
			DEBUG(10,("smbd_check_access_rights: acl for %s is:\n",
				smb_fname_str_dbg(smb_fname) ));
			NDR_PRINT_DEBUG(security_descriptor, sd);
		}
	}

	TALLOC_FREE(sd);

	if (NT_STATUS_IS_OK(status) ||
			!NT_STATUS_EQUAL(status, NT_STATUS_ACCESS_DENIED)) {
		return status;
	}

	/* Here we know status == NT_STATUS_ACCESS_DENIED. */

  access_denied:

	if ((access_mask & FILE_WRITE_ATTRIBUTES) &&
			(rejected_mask & FILE_WRITE_ATTRIBUTES) &&
			!lp_store_dos_attributes(SNUM(conn)) &&
			(lp_map_readonly(SNUM(conn)) ||
			lp_map_archive(SNUM(conn)) ||
			lp_map_hidden(SNUM(conn)) ||
			lp_map_system(SNUM(conn)))) {
		rejected_mask &= ~FILE_WRITE_ATTRIBUTES;

		DEBUG(10,("smbd_check_access_rights: "
			"overrode "
			"FILE_WRITE_ATTRIBUTES "
			"on file %s\n",
			smb_fname_str_dbg(smb_fname)));
	}

	if (parent_override_delete(conn,
				smb_fname,
				access_mask,
				rejected_mask)) {
		/* Were we trying to do an open
		 * for delete and didn't get DELETE
		 * access (only) ? Check if the
		 * directory allows DELETE_CHILD.
		 * See here:
		 * http://blogs.msdn.com/oldnewthing/archive/2004/06/04/148426.aspx
		 * for details. */

		rejected_mask &= ~DELETE_ACCESS;

		DEBUG(10,("smbd_check_access_rights: "
			"overrode "
			"DELETE_ACCESS on "
			"file %s\n",
			smb_fname_str_dbg(smb_fname)));
	}

	if (rejected_mask != 0) {
		return NT_STATUS_ACCESS_DENIED;
	}
	return NT_STATUS_OK;
}