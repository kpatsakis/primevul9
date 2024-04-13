static NTSTATUS receive_smb_raw_talloc_partial_read(TALLOC_CTX *mem_ctx,
						    const char lenbuf[4],
						    int fd, char **buffer,
						    unsigned int timeout,
						    size_t *p_unread,
						    size_t *len_ret)
{
	/* Size of a WRITEX call (+4 byte len). */
	char writeX_header[4 + STANDARD_WRITE_AND_X_HEADER_SIZE];
	ssize_t len = smb_len_large(lenbuf); /* Could be a UNIX large writeX. */
	ssize_t toread;
	NTSTATUS status;

	memcpy(writeX_header, lenbuf, 4);

	status = read_socket_with_timeout(
		fd, writeX_header + 4,
		STANDARD_WRITE_AND_X_HEADER_SIZE,
		STANDARD_WRITE_AND_X_HEADER_SIZE,
		timeout, NULL);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	/*
	 * Ok - now try and see if this is a possible
	 * valid writeX call.
	 */

	if (is_valid_writeX_buffer((uint8_t *)writeX_header)) {
		/*
		 * If the data offset is beyond what
		 * we've read, drain the extra bytes.
		 */
		uint16_t doff = SVAL(writeX_header,smb_vwv11);
		ssize_t newlen;

		if (doff > STANDARD_WRITE_AND_X_HEADER_SIZE) {
			size_t drain = doff - STANDARD_WRITE_AND_X_HEADER_SIZE;
			if (drain_socket(smbd_server_fd(), drain) != drain) {
	                        smb_panic("receive_smb_raw_talloc_partial_read:"
					" failed to drain pending bytes");
	                }
		} else {
			doff = STANDARD_WRITE_AND_X_HEADER_SIZE;
		}

		/* Spoof down the length and null out the bcc. */
		set_message_bcc(writeX_header, 0);
		newlen = smb_len(writeX_header);

		/* Copy the header we've written. */

		*buffer = (char *)TALLOC_MEMDUP(mem_ctx,
				writeX_header,
				sizeof(writeX_header));

		if (*buffer == NULL) {
			DEBUG(0, ("Could not allocate inbuf of length %d\n",
				  (int)sizeof(writeX_header)));
			return NT_STATUS_NO_MEMORY;
		}

		/* Work out the remaining bytes. */
		*p_unread = len - STANDARD_WRITE_AND_X_HEADER_SIZE;
		*len_ret = newlen + 4;
		return NT_STATUS_OK;
	}

	if (!valid_packet_size(len)) {
		return NT_STATUS_INVALID_PARAMETER;
	}

	/*
	 * Not a valid writeX call. Just do the standard
	 * talloc and return.
	 */

	*buffer = TALLOC_ARRAY(mem_ctx, char, len+4);

	if (*buffer == NULL) {
		DEBUG(0, ("Could not allocate inbuf of length %d\n",
			  (int)len+4));
		return NT_STATUS_NO_MEMORY;
	}

	/* Copy in what we already read. */
	memcpy(*buffer,
		writeX_header,
		4 + STANDARD_WRITE_AND_X_HEADER_SIZE);
	toread = len - STANDARD_WRITE_AND_X_HEADER_SIZE;

	if(toread > 0) {
		status = read_packet_remainder(
			fd, (*buffer) + 4 + STANDARD_WRITE_AND_X_HEADER_SIZE,
			timeout, toread);

		if (!NT_STATUS_IS_OK(status)) {
			DEBUG(10, ("receive_smb_raw_talloc_partial_read: %s\n",
				   nt_errstr(status)));
			return status;
		}
	}

	*len_ret = len + 4;
	return NT_STATUS_OK;
}