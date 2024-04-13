static NTSTATUS fcb_or_dos_open(struct smb_request *req,
				connection_struct *conn,
				files_struct *fsp_to_dup_into,
				const struct smb_filename *smb_fname,
				struct file_id id,
				uint16 file_pid,
				uint64_t vuid,
				uint32 access_mask,
				uint32 share_access,
				uint32 create_options)
{
	files_struct *fsp;

	DEBUG(5,("fcb_or_dos_open: attempting old open semantics for "
		 "file %s.\n", smb_fname_str_dbg(smb_fname)));

	for(fsp = file_find_di_first(conn->sconn, id); fsp;
	    fsp = file_find_di_next(fsp)) {

		DEBUG(10,("fcb_or_dos_open: checking file %s, fd = %d, "
			  "vuid = %llu, file_pid = %u, private_options = 0x%x "
			  "access_mask = 0x%x\n", fsp_str_dbg(fsp),
			  fsp->fh->fd, (unsigned long long)fsp->vuid,
			  (unsigned int)fsp->file_pid,
			  (unsigned int)fsp->fh->private_options,
			  (unsigned int)fsp->access_mask ));

		if (fsp != fsp_to_dup_into &&
		    fsp->fh->fd != -1 &&
		    fsp->vuid == vuid &&
		    fsp->file_pid == file_pid &&
		    (fsp->fh->private_options & (NTCREATEX_OPTIONS_PRIVATE_DENY_DOS |
						 NTCREATEX_OPTIONS_PRIVATE_DENY_FCB)) &&
		    (fsp->access_mask & FILE_WRITE_DATA) &&
		    strequal(fsp->fsp_name->base_name, smb_fname->base_name) &&
		    strequal(fsp->fsp_name->stream_name,
			     smb_fname->stream_name)) {
			DEBUG(10,("fcb_or_dos_open: file match\n"));
			break;
		}
	}

	if (!fsp) {
		return NT_STATUS_NOT_FOUND;
	}

	/* quite an insane set of semantics ... */
	if (is_executable(smb_fname->base_name) &&
	    (fsp->fh->private_options & NTCREATEX_OPTIONS_PRIVATE_DENY_DOS)) {
		DEBUG(10,("fcb_or_dos_open: file fail due to is_executable.\n"));
		return NT_STATUS_INVALID_PARAMETER;
	}

	/* We need to duplicate this fsp. */
	return dup_file_fsp(req, fsp, access_mask, share_access,
			    create_options, fsp_to_dup_into);
}