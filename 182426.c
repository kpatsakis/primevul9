smb2_plain_req_init(__le16 smb2_command, struct cifs_tcon *tcon,
		    void **request_buf, unsigned int *total_len)
{
	int rc;

	rc = smb2_reconnect(smb2_command, tcon);
	if (rc)
		return rc;

	/* BB eventually switch this to SMB2 specific small buf size */
	if (smb2_command == SMB2_SET_INFO)
		*request_buf = cifs_buf_get();
	else
		*request_buf = cifs_small_buf_get();
	if (*request_buf == NULL) {
		/* BB should we add a retry in here if not a writepage? */
		return -ENOMEM;
	}

	fill_small_buf(smb2_command, tcon,
		       (struct smb2_sync_hdr *)(*request_buf),
		       total_len);

	if (tcon != NULL) {
		uint16_t com_code = le16_to_cpu(smb2_command);
		cifs_stats_inc(&tcon->stats.smb2_stats.smb2_com_sent[com_code]);
		cifs_stats_inc(&tcon->num_smbs_sent);
	}

	return rc;
}