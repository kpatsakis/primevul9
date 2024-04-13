SMB2_query_info_init(struct cifs_tcon *tcon, struct smb_rqst *rqst,
		     u64 persistent_fid, u64 volatile_fid,
		     u8 info_class, u8 info_type, u32 additional_info,
		     size_t output_len, size_t input_len, void *input)
{
	struct smb2_query_info_req *req;
	struct kvec *iov = rqst->rq_iov;
	unsigned int total_len;
	int rc;

	rc = smb2_plain_req_init(SMB2_QUERY_INFO, tcon, (void **) &req,
			     &total_len);
	if (rc)
		return rc;

	req->InfoType = info_type;
	req->FileInfoClass = info_class;
	req->PersistentFileId = persistent_fid;
	req->VolatileFileId = volatile_fid;
	req->AdditionalInformation = cpu_to_le32(additional_info);

	req->OutputBufferLength = cpu_to_le32(output_len);
	if (input_len) {
		req->InputBufferLength = cpu_to_le32(input_len);
		/* total_len for smb query request never close to le16 max */
		req->InputBufferOffset = cpu_to_le16(total_len - 1);
		memcpy(req->Buffer, input, input_len);
	}

	iov[0].iov_base = (char *)req;
	/* 1 for Buffer */
	iov[0].iov_len = total_len - 1 + input_len;
	return 0;
}