void msg_file_was_renamed(struct messaging_context *msg,
			  void *private_data,
			  uint32_t msg_type,
			  struct server_id server_id,
			  DATA_BLOB *data)
{
	files_struct *fsp;
	char *frm = (char *)data->data;
	struct file_id id;
	const char *sharepath;
	const char *base_name;
	const char *stream_name;
	struct smb_filename *smb_fname = NULL;
	size_t sp_len, bn_len;
	NTSTATUS status;
	struct smbd_server_connection *sconn =
		talloc_get_type_abort(private_data,
		struct smbd_server_connection);

	if (data->data == NULL
	    || data->length < MSG_FILE_RENAMED_MIN_SIZE + 2) {
                DEBUG(0, ("msg_file_was_renamed: Got invalid msg len %d\n",
			  (int)data->length));
                return;
        }

	/* Unpack the message. */
	pull_file_id_24(frm, &id);
	sharepath = &frm[24];
	sp_len = strlen(sharepath);
	base_name = sharepath + sp_len + 1;
	bn_len = strlen(base_name);
	stream_name = sharepath + sp_len + 1 + bn_len + 1;

	/* stream_name must always be NULL if there is no stream. */
	if (stream_name[0] == '\0') {
		stream_name = NULL;
	}

	smb_fname = synthetic_smb_fname(talloc_tos(), base_name,
					stream_name, NULL);
	if (smb_fname == NULL) {
		return;
	}

	DEBUG(10,("msg_file_was_renamed: Got rename message for sharepath %s, new name %s, "
		"file_id %s\n",
		sharepath, smb_fname_str_dbg(smb_fname),
		file_id_string_tos(&id)));

	for(fsp = file_find_di_first(sconn, id); fsp;
	    fsp = file_find_di_next(fsp)) {
		if (memcmp(fsp->conn->connectpath, sharepath, sp_len) == 0) {

			DEBUG(10,("msg_file_was_renamed: renaming file %s from %s -> %s\n",
				fsp_fnum_dbg(fsp), fsp_str_dbg(fsp),
				smb_fname_str_dbg(smb_fname)));
			status = fsp_set_smb_fname(fsp, smb_fname);
			if (!NT_STATUS_IS_OK(status)) {
				goto out;
			}
		} else {
			/* TODO. JRA. */
			/* Now we have the complete path we can work out if this is
			   actually within this share and adjust newname accordingly. */
	                DEBUG(10,("msg_file_was_renamed: share mismatch (sharepath %s "
				"not sharepath %s) "
				"%s from %s -> %s\n",
				fsp->conn->connectpath,
				sharepath,
				fsp_fnum_dbg(fsp),
				fsp_str_dbg(fsp),
				smb_fname_str_dbg(smb_fname)));
		}
        }
 out:
	TALLOC_FREE(smb_fname);
	return;
}