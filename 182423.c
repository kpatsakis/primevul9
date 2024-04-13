add_twarp_context(struct kvec *iov, unsigned int *num_iovec, __u64 timewarp)
{
	struct smb2_create_req *req = iov[0].iov_base;
	unsigned int num = *num_iovec;

	iov[num].iov_base = create_twarp_buf(timewarp);
	if (iov[num].iov_base == NULL)
		return -ENOMEM;
	iov[num].iov_len = sizeof(struct crt_twarp_ctxt);
	if (!req->CreateContextsOffset)
		req->CreateContextsOffset = cpu_to_le32(
				sizeof(struct smb2_create_req) +
				iov[num - 1].iov_len);
	le32_add_cpu(&req->CreateContextsLength, sizeof(struct crt_twarp_ctxt));
	*num_iovec = num + 1;
	return 0;
}