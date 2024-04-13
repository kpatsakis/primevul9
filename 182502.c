add_lease_context(struct TCP_Server_Info *server, struct kvec *iov,
		  unsigned int *num_iovec, u8 *lease_key, __u8 *oplock)
{
	struct smb2_create_req *req = iov[0].iov_base;
	unsigned int num = *num_iovec;

	iov[num].iov_base = server->ops->create_lease_buf(lease_key, *oplock);
	if (iov[num].iov_base == NULL)
		return -ENOMEM;
	iov[num].iov_len = server->vals->create_lease_size;
	req->RequestedOplockLevel = SMB2_OPLOCK_LEVEL_LEASE;
	if (!req->CreateContextsOffset)
		req->CreateContextsOffset = cpu_to_le32(
				sizeof(struct smb2_create_req) +
				iov[num - 1].iov_len);
	le32_add_cpu(&req->CreateContextsLength,
		     server->vals->create_lease_size);
	*num_iovec = num + 1;
	return 0;
}