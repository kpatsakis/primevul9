static void smbd_server_connection_read_handler(struct smbd_server_connection *conn)
{
	uint8_t *inbuf = NULL;
	size_t inbuf_len = 0;
	size_t unread_bytes = 0;
	bool encrypted = false;
	TALLOC_CTX *mem_ctx = talloc_tos();
	NTSTATUS status;
	uint32_t seqnum;

	/* TODO: make this completely nonblocking */

	status = receive_smb_talloc(mem_ctx, smbd_server_fd(),
				    (char **)(void *)&inbuf,
				    0, /* timeout */
				    &unread_bytes,
				    &encrypted,
				    &inbuf_len, &seqnum);
	if (NT_STATUS_EQUAL(status, NT_STATUS_RETRY)) {
		goto process;
	}
	if (NT_STATUS_IS_ERR(status)) {
		exit_server_cleanly("failed to receive smb request");
	}
	if (!NT_STATUS_IS_OK(status)) {
		return;
	}

process:
	process_smb(conn, inbuf, inbuf_len, unread_bytes,
		    seqnum, encrypted, NULL);
}