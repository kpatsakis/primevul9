SMB2_sess_free_buffer(struct SMB2_sess_data *sess_data)
{
	free_rsp_buf(sess_data->buf0_type, sess_data->iov[0].iov_base);
	sess_data->buf0_type = CIFS_NO_BUFFER;
}