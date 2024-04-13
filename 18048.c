NTSTATUS open_file_fchmod(connection_struct *conn,
			  struct smb_filename *smb_fname,
			  files_struct **result)
{
	if (!VALID_STAT(smb_fname->st)) {
		return NT_STATUS_INVALID_PARAMETER;
	}

        return SMB_VFS_CREATE_FILE(
		conn,					/* conn */
		NULL,					/* req */
		0,					/* root_dir_fid */
		smb_fname,				/* fname */
		FILE_WRITE_DATA,			/* access_mask */
		(FILE_SHARE_READ | FILE_SHARE_WRITE |	/* share_access */
		    FILE_SHARE_DELETE),
		FILE_OPEN,				/* create_disposition*/
		0,					/* create_options */
		0,					/* file_attributes */
		INTERNAL_OPEN_ONLY,			/* oplock_request */
		0,					/* allocation_size */
		0,					/* private_flags */
		NULL,					/* sd */
		NULL,					/* ea_list */
		result,					/* result */
		NULL);					/* pinfo */
}