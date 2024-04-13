create_twarp_buf(__u64 timewarp)
{
	struct crt_twarp_ctxt *buf;

	buf = kzalloc(sizeof(struct crt_twarp_ctxt), GFP_KERNEL);
	if (!buf)
		return NULL;

	buf->ccontext.DataOffset = cpu_to_le16(offsetof
					(struct crt_twarp_ctxt, Timestamp));
	buf->ccontext.DataLength = cpu_to_le32(8);
	buf->ccontext.NameOffset = cpu_to_le16(offsetof
				(struct crt_twarp_ctxt, Name));
	buf->ccontext.NameLength = cpu_to_le16(4);
	/* SMB2_CREATE_TIMEWARP_TOKEN is "TWrp" */
	buf->Name[0] = 'T';
	buf->Name[1] = 'W';
	buf->Name[2] = 'r';
	buf->Name[3] = 'p';
	buf->Timestamp = cpu_to_le64(timewarp);
	return buf;
}