create_reconnect_durable_buf(struct cifs_fid *fid)
{
	struct create_durable *buf;

	buf = kzalloc(sizeof(struct create_durable), GFP_KERNEL);
	if (!buf)
		return NULL;

	buf->ccontext.DataOffset = cpu_to_le16(offsetof
					(struct create_durable, Data));
	buf->ccontext.DataLength = cpu_to_le32(16);
	buf->ccontext.NameOffset = cpu_to_le16(offsetof
				(struct create_durable, Name));
	buf->ccontext.NameLength = cpu_to_le16(4);
	buf->Data.Fid.PersistentFileId = fid->persistent_fid;
	buf->Data.Fid.VolatileFileId = fid->volatile_fid;
	/* SMB2_CREATE_DURABLE_HANDLE_RECONNECT is "DHnC" */
	buf->Name[0] = 'D';
	buf->Name[1] = 'H';
	buf->Name[2] = 'n';
	buf->Name[3] = 'C';
	return buf;
}