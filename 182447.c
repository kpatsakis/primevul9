add_durable_reconnect_v2_context(struct kvec *iov, unsigned int *num_iovec,
		    struct cifs_open_parms *oparms)
{
	struct smb2_create_req *req = iov[0].iov_base;
	unsigned int num = *num_iovec;

	/* indicate that we don't need to relock the file */
	oparms->reconnect = false;

	iov[num].iov_base = create_reconnect_durable_v2_buf(oparms->fid);
	if (iov[num].iov_base == NULL)
		return -ENOMEM;
	iov[num].iov_len = sizeof(struct create_durable_handle_reconnect_v2);
	if (!req->CreateContextsOffset)
		req->CreateContextsOffset =
			cpu_to_le32(sizeof(struct smb2_create_req) +
								iov[1].iov_len);
	le32_add_cpu(&req->CreateContextsLength,
			sizeof(struct create_durable_handle_reconnect_v2));
	*num_iovec = num + 1;
	return 0;
}