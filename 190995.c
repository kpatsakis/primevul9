static NTSTATUS receive_smb_talloc(TALLOC_CTX *mem_ctx,	int fd,
				   char **buffer, unsigned int timeout,
				   size_t *p_unread, bool *p_encrypted,
				   size_t *p_len,
				   uint32_t *seqnum)
{
	size_t len = 0;
	NTSTATUS status;

	*p_encrypted = false;

	status = receive_smb_raw_talloc(mem_ctx, fd, buffer, timeout,
					p_unread, &len);
	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (is_encrypted_packet((uint8_t *)*buffer)) {
		status = srv_decrypt_buffer(*buffer);
		if (!NT_STATUS_IS_OK(status)) {
			DEBUG(0, ("receive_smb_talloc: SMB decryption failed on "
				"incoming packet! Error %s\n",
				nt_errstr(status) ));
			return status;
		}
		*p_encrypted = true;
	}

	/* Check the incoming SMB signature. */
	if (!srv_check_sign_mac(smbd_server_conn, *buffer, seqnum)) {
		DEBUG(0, ("receive_smb: SMB Signature verification failed on "
			  "incoming packet!\n"));
		return NT_STATUS_INVALID_NETWORK_RESPONSE;
	}

	*p_len = len;
	return NT_STATUS_OK;
}