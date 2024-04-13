bool srv_send_smb(int fd, char *buffer,
		  bool do_signing, uint32_t seqnum,
		  bool do_encrypt,
		  struct smb_perfcount_data *pcd)
{
	size_t len = 0;
	size_t nwritten=0;
	ssize_t ret;
	char *buf_out = buffer;

	if (do_signing) {
		/* Sign the outgoing packet if required. */
		srv_calculate_sign_mac(smbd_server_conn, buf_out, seqnum);
	}

	if (do_encrypt) {
		NTSTATUS status = srv_encrypt_buffer(buffer, &buf_out);
		if (!NT_STATUS_IS_OK(status)) {
			DEBUG(0, ("send_smb: SMB encryption failed "
				"on outgoing packet! Error %s\n",
				nt_errstr(status) ));
			goto out;
		}
	}

	len = smb_len(buf_out) + 4;

	ret = write_data(fd,buf_out+nwritten,len - nwritten);
	if (ret <= 0) {
		DEBUG(0,("Error writing %d bytes to client. %d. (%s)\n",
			 (int)len,(int)ret, strerror(errno) ));
		srv_free_enc_buffer(buf_out);
		goto out;
	}

	SMB_PERFCOUNT_SET_MSGLEN_OUT(pcd, len);
	srv_free_enc_buffer(buf_out);
out:
	SMB_PERFCOUNT_END(pcd);
	return true;
}