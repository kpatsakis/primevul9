NTSTATUS get_relative_fid_filename(connection_struct *conn,
				   struct smb_request *req,
				   uint16_t root_dir_fid,
				   const struct smb_filename *smb_fname,
				   struct smb_filename **smb_fname_out)
{
	files_struct *dir_fsp;
	char *parent_fname = NULL;
	char *new_base_name = NULL;
	NTSTATUS status;

	if (root_dir_fid == 0 || !smb_fname) {
		status = NT_STATUS_INTERNAL_ERROR;
		goto out;
	}

	dir_fsp = file_fsp(req, root_dir_fid);

	if (dir_fsp == NULL) {
		status = NT_STATUS_INVALID_HANDLE;
		goto out;
	}

	if (is_ntfs_stream_smb_fname(dir_fsp->fsp_name)) {
		status = NT_STATUS_INVALID_HANDLE;
		goto out;
	}

	if (!dir_fsp->is_directory) {

		/*
		 * Check to see if this is a mac fork of some kind.
		 */

		if ((conn->fs_capabilities & FILE_NAMED_STREAMS) &&
		    is_ntfs_stream_smb_fname(smb_fname)) {
			status = NT_STATUS_OBJECT_PATH_NOT_FOUND;
			goto out;
		}

		/*
		  we need to handle the case when we get a
		  relative open relative to a file and the
		  pathname is blank - this is a reopen!
		  (hint from demyn plantenberg)
		*/

		status = NT_STATUS_INVALID_HANDLE;
		goto out;
	}

	if (ISDOT(dir_fsp->fsp_name->base_name)) {
		/*
		 * We're at the toplevel dir, the final file name
		 * must not contain ./, as this is filtered out
		 * normally by srvstr_get_path and unix_convert
		 * explicitly rejects paths containing ./.
		 */
		parent_fname = talloc_strdup(talloc_tos(), "");
		if (parent_fname == NULL) {
			status = NT_STATUS_NO_MEMORY;
			goto out;
		}
	} else {
		size_t dir_name_len = strlen(dir_fsp->fsp_name->base_name);

		/*
		 * Copy in the base directory name.
		 */

		parent_fname = talloc_array(talloc_tos(), char,
		    dir_name_len+2);
		if (parent_fname == NULL) {
			status = NT_STATUS_NO_MEMORY;
			goto out;
		}
		memcpy(parent_fname, dir_fsp->fsp_name->base_name,
		    dir_name_len+1);

		/*
		 * Ensure it ends in a '/'.
		 * We used TALLOC_SIZE +2 to add space for the '/'.
		 */

		if(dir_name_len
		    && (parent_fname[dir_name_len-1] != '\\')
		    && (parent_fname[dir_name_len-1] != '/')) {
			parent_fname[dir_name_len] = '/';
			parent_fname[dir_name_len+1] = '\0';
		}
	}

	new_base_name = talloc_asprintf(talloc_tos(), "%s%s", parent_fname,
					smb_fname->base_name);
	if (new_base_name == NULL) {
		status = NT_STATUS_NO_MEMORY;
		goto out;
	}

	status = filename_convert(req,
				conn,
				req->flags2 & FLAGS2_DFS_PATHNAMES,
				new_base_name,
				0,
				NULL,
				smb_fname_out);
	if (!NT_STATUS_IS_OK(status)) {
		goto out;
	}

 out:
	TALLOC_FREE(parent_fname);
	TALLOC_FREE(new_base_name);
	return status;
}