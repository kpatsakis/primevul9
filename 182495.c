SMB2_sess_sendreceive(struct SMB2_sess_data *sess_data)
{
	int rc;
	struct smb_rqst rqst;
	struct smb2_sess_setup_req *req = sess_data->iov[0].iov_base;
	struct kvec rsp_iov = { NULL, 0 };

	/* Testing shows that buffer offset must be at location of Buffer[0] */
	req->SecurityBufferOffset =
		cpu_to_le16(sizeof(struct smb2_sess_setup_req) - 1 /* pad */);
	req->SecurityBufferLength = cpu_to_le16(sess_data->iov[1].iov_len);

	memset(&rqst, 0, sizeof(struct smb_rqst));
	rqst.rq_iov = sess_data->iov;
	rqst.rq_nvec = 2;

	/* BB add code to build os and lm fields */
	rc = cifs_send_recv(sess_data->xid, sess_data->ses,
			    &rqst,
			    &sess_data->buf0_type,
			    CIFS_LOG_ERROR | CIFS_NEG_OP, &rsp_iov);
	cifs_small_buf_release(sess_data->iov[0].iov_base);
	memcpy(&sess_data->iov[0], &rsp_iov, sizeof(struct kvec));

	return rc;
}