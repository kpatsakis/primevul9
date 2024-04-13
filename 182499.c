add_posix_context(struct kvec *iov, unsigned int *num_iovec, umode_t mode)
{
	struct smb2_create_req *req = iov[0].iov_base;
	unsigned int num = *num_iovec;

	iov[num].iov_base = create_posix_buf(mode);
	if (iov[num].iov_base == NULL)
		return -ENOMEM;
	iov[num].iov_len = sizeof(struct create_posix);
	if (!req->CreateContextsOffset)
		req->CreateContextsOffset = cpu_to_le32(
				sizeof(struct smb2_create_req) +
				iov[num - 1].iov_len);
	le32_add_cpu(&req->CreateContextsLength, sizeof(struct create_posix));
	*num_iovec = num + 1;
	return 0;
}