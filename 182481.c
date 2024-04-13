SMB2_echo(struct TCP_Server_Info *server)
{
	struct smb2_echo_req *req;
	int rc = 0;
	struct kvec iov[1];
	struct smb_rqst rqst = { .rq_iov = iov,
				 .rq_nvec = 1 };
	unsigned int total_len;

	cifs_dbg(FYI, "In echo request\n");

	if (server->tcpStatus == CifsNeedNegotiate) {
		/* No need to send echo on newly established connections */
		queue_delayed_work(cifsiod_wq, &server->reconnect, 0);
		return rc;
	}

	rc = smb2_plain_req_init(SMB2_ECHO, NULL, (void **)&req, &total_len);
	if (rc)
		return rc;

	req->sync_hdr.CreditRequest = cpu_to_le16(1);

	iov[0].iov_len = total_len;
	iov[0].iov_base = (char *)req;

	rc = cifs_call_async(server, &rqst, NULL, smb2_echo_callback, NULL,
			     server, CIFS_ECHO_OP, NULL);
	if (rc)
		cifs_dbg(FYI, "Echo request failed: %d\n", rc);

	cifs_small_buf_release(req);
	return rc;
}