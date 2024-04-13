SMB2_ioctl_init(struct cifs_tcon *tcon, struct smb_rqst *rqst,
		u64 persistent_fid, u64 volatile_fid, u32 opcode,
		bool is_fsctl, char *in_data, u32 indatalen,
		__u32 max_response_size)
{
	struct smb2_ioctl_req *req;
	struct kvec *iov = rqst->rq_iov;
	unsigned int total_len;
	int rc;

	rc = smb2_plain_req_init(SMB2_IOCTL, tcon, (void **) &req, &total_len);
	if (rc)
		return rc;

	req->CtlCode = cpu_to_le32(opcode);
	req->PersistentFileId = persistent_fid;
	req->VolatileFileId = volatile_fid;

	iov[0].iov_base = (char *)req;
	/*
	 * If no input data, the size of ioctl struct in
	 * protocol spec still includes a 1 byte data buffer,
	 * but if input data passed to ioctl, we do not
	 * want to double count this, so we do not send
	 * the dummy one byte of data in iovec[0] if sending
	 * input data (in iovec[1]).
	 */
	if (indatalen) {
		req->InputCount = cpu_to_le32(indatalen);
		/* do not set InputOffset if no input data */
		req->InputOffset =
		       cpu_to_le32(offsetof(struct smb2_ioctl_req, Buffer));
		rqst->rq_nvec = 2;
		iov[0].iov_len = total_len - 1;
		iov[1].iov_base = in_data;
		iov[1].iov_len = indatalen;
	} else {
		rqst->rq_nvec = 1;
		iov[0].iov_len = total_len;
	}

	req->OutputOffset = 0;
	req->OutputCount = 0; /* MBZ */

	/*
	 * In most cases max_response_size is set to 16K (CIFSMaxBufSize)
	 * We Could increase default MaxOutputResponse, but that could require
	 * more credits. Windows typically sets this smaller, but for some
	 * ioctls it may be useful to allow server to send more. No point
	 * limiting what the server can send as long as fits in one credit
	 * We can not handle more than CIFS_MAX_BUF_SIZE yet but may want
	 * to increase this limit up in the future.
	 * Note that for snapshot queries that servers like Azure expect that
	 * the first query be minimal size (and just used to get the number/size
	 * of previous versions) so response size must be specified as EXACTLY
	 * sizeof(struct snapshot_array) which is 16 when rounded up to multiple
	 * of eight bytes.  Currently that is the only case where we set max
	 * response size smaller.
	 */
	req->MaxOutputResponse = cpu_to_le32(max_response_size);

	if (is_fsctl)
		req->Flags = cpu_to_le32(SMB2_0_IOCTL_IS_FSCTL);
	else
		req->Flags = 0;

	/* validate negotiate request must be signed - see MS-SMB2 3.2.5.5 */
	if (opcode == FSCTL_VALIDATE_NEGOTIATE_INFO)
		req->sync_hdr.Flags |= SMB2_FLAGS_SIGNED;

	return 0;
}