static void validate_my_share_entries(struct smbd_server_connection *sconn,
				      int num,
				      struct share_mode_entry *share_entry)
{
	struct server_id self = messaging_server_id(sconn->msg_ctx);
	files_struct *fsp;

	if (!serverid_equal(&self, &share_entry->pid)) {
		return;
	}

	if (!is_valid_share_mode_entry(share_entry)) {
		return;
	}

	fsp = file_find_dif(sconn, share_entry->id,
			    share_entry->share_file_id);
	if (!fsp) {
		DEBUG(0,("validate_my_share_entries: PANIC : %s\n",
			 share_mode_str(talloc_tos(), num, share_entry) ));
		smb_panic("validate_my_share_entries: Cannot match a "
			  "share entry with an open file\n");
	}

	if (((uint16)fsp->oplock_type) != share_entry->op_type) {
		goto panic;
	}

	return;

 panic:
	{
		char *str;
		DEBUG(0,("validate_my_share_entries: PANIC : %s\n",
			 share_mode_str(talloc_tos(), num, share_entry) ));
		str = talloc_asprintf(talloc_tos(),
			"validate_my_share_entries: "
			"file %s, oplock_type = 0x%x, op_type = 0x%x\n",
			 fsp->fsp_name->base_name,
			 (unsigned int)fsp->oplock_type,
			 (unsigned int)share_entry->op_type );
		smb_panic(str);
	}
}