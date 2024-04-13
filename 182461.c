SMB2_set_info_init(struct cifs_tcon *tcon, struct smb_rqst *rqst,
	       u64 persistent_fid, u64 volatile_fid, u32 pid, u8 info_class,
	       u8 info_type, u32 additional_info,
		void **data, unsigned int *size)
{
	struct smb2_set_info_req *req;
	struct kvec *iov = rqst->rq_iov;
	unsigned int i, total_len;
	int rc;

	rc = smb2_plain_req_init(SMB2_SET_INFO, tcon, (void **) &req, &total_len);
	if (rc)
		return rc;

	req->sync_hdr.ProcessId = cpu_to_le32(pid);
	req->InfoType = info_type;
	req->FileInfoClass = info_class;
	req->PersistentFileId = persistent_fid;
	req->VolatileFileId = volatile_fid;
	req->AdditionalInformation = cpu_to_le32(additional_info);

	req->BufferOffset =
			cpu_to_le16(sizeof(struct smb2_set_info_req) - 1);
	req->BufferLength = cpu_to_le32(*size);

	memcpy(req->Buffer, *data, *size);
	total_len += *size;

	iov[0].iov_base = (char *)req;
	/* 1 for Buffer */
	iov[0].iov_len = total_len - 1;

	for (i = 1; i < rqst->rq_nvec; i++) {
		le32_add_cpu(&req->BufferLength, size[i]);
		iov[i].iov_base = (char *)data[i];
		iov[i].iov_len = size[i];
	}

	return 0;
}