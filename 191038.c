static NTSTATUS receive_smb_raw_talloc(TALLOC_CTX *mem_ctx, int fd,
				       char **buffer, unsigned int timeout,
				       size_t *p_unread, size_t *plen)
{
	char lenbuf[4];
	size_t len;
	int min_recv_size = lp_min_receive_file_size();
	NTSTATUS status;

	*p_unread = 0;

	status = read_smb_length_return_keepalive(fd, lenbuf, timeout, &len);
	if (!NT_STATUS_IS_OK(status)) {
		DEBUG(10, ("receive_smb_raw: %s\n", nt_errstr(status)));
		return status;
	}

	if (CVAL(lenbuf,0) == 0 && min_recv_size &&
	    (smb_len_large(lenbuf) > /* Could be a UNIX large writeX. */
		(min_recv_size + STANDARD_WRITE_AND_X_HEADER_SIZE)) &&
	    !srv_is_signing_active(smbd_server_conn)) {

		return receive_smb_raw_talloc_partial_read(
			mem_ctx, lenbuf, fd, buffer, timeout, p_unread, plen);
	}

	if (!valid_packet_size(len)) {
		return NT_STATUS_INVALID_PARAMETER;
	}

	/*
	 * The +4 here can't wrap, we've checked the length above already.
	 */

	*buffer = TALLOC_ARRAY(mem_ctx, char, len+4);

	if (*buffer == NULL) {
		DEBUG(0, ("Could not allocate inbuf of length %d\n",
			  (int)len+4));
		return NT_STATUS_NO_MEMORY;
	}

	memcpy(*buffer, lenbuf, sizeof(lenbuf));

	status = read_packet_remainder(fd, (*buffer)+4, timeout, len);
	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	*plen = len + 4;
	return NT_STATUS_OK;
}