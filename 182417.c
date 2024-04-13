create_reconnect_durable_v2_buf(struct cifs_fid *fid)
{
	struct create_durable_handle_reconnect_v2 *buf;

	buf = kzalloc(sizeof(struct create_durable_handle_reconnect_v2),
			GFP_KERNEL);
	if (!buf)
		return NULL;

	buf->ccontext.DataOffset =
		cpu_to_le16(offsetof(struct create_durable_handle_reconnect_v2,
				     dcontext));
	buf->ccontext.DataLength =
		cpu_to_le32(sizeof(struct durable_reconnect_context_v2));
	buf->ccontext.NameOffset =
		cpu_to_le16(offsetof(struct create_durable_handle_reconnect_v2,
			    Name));
	buf->ccontext.NameLength = cpu_to_le16(4);

	buf->dcontext.Fid.PersistentFileId = fid->persistent_fid;
	buf->dcontext.Fid.VolatileFileId = fid->volatile_fid;
	buf->dcontext.Flags = cpu_to_le32(SMB2_DHANDLE_FLAG_PERSISTENT);
	memcpy(buf->dcontext.CreateGuid, fid->create_guid, 16);

	/* SMB2_CREATE_DURABLE_HANDLE_RECONNECT_V2 is "DH2C" */
	buf->Name[0] = 'D';
	buf->Name[1] = 'H';
	buf->Name[2] = '2';
	buf->Name[3] = 'C';
	return buf;
}