add_durable_context(struct kvec *iov, unsigned int *num_iovec,
		    struct cifs_open_parms *oparms, bool use_persistent)
{
	struct smb2_create_req *req = iov[0].iov_base;
	unsigned int num = *num_iovec;

	if (use_persistent) {
		if (oparms->reconnect)
			return add_durable_reconnect_v2_context(iov, num_iovec,
								oparms);
		else
			return add_durable_v2_context(iov, num_iovec, oparms);
	}

	if (oparms->reconnect) {
		iov[num].iov_base = create_reconnect_durable_buf(oparms->fid);
		/* indicate that we don't need to relock the file */
		oparms->reconnect = false;
	} else
		iov[num].iov_base = create_durable_buf();
	if (iov[num].iov_base == NULL)
		return -ENOMEM;
	iov[num].iov_len = sizeof(struct create_durable);
	if (!req->CreateContextsOffset)
		req->CreateContextsOffset =
			cpu_to_le32(sizeof(struct smb2_create_req) +
								iov[1].iov_len);
	le32_add_cpu(&req->CreateContextsLength, sizeof(struct create_durable));
	*num_iovec = num + 1;
	return 0;
}