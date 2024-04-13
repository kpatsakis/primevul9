create_posix_buf(umode_t mode)
{
	struct create_posix *buf;

	buf = kzalloc(sizeof(struct create_posix),
			GFP_KERNEL);
	if (!buf)
		return NULL;

	buf->ccontext.DataOffset =
		cpu_to_le16(offsetof(struct create_posix, Mode));
	buf->ccontext.DataLength = cpu_to_le32(4);
	buf->ccontext.NameOffset =
		cpu_to_le16(offsetof(struct create_posix, Name));
	buf->ccontext.NameLength = cpu_to_le16(16);

	/* SMB2_CREATE_TAG_POSIX is "0x93AD25509CB411E7B42383DE968BCD7C" */
	buf->Name[0] = 0x93;
	buf->Name[1] = 0xAD;
	buf->Name[2] = 0x25;
	buf->Name[3] = 0x50;
	buf->Name[4] = 0x9C;
	buf->Name[5] = 0xB4;
	buf->Name[6] = 0x11;
	buf->Name[7] = 0xE7;
	buf->Name[8] = 0xB4;
	buf->Name[9] = 0x23;
	buf->Name[10] = 0x83;
	buf->Name[11] = 0xDE;
	buf->Name[12] = 0x96;
	buf->Name[13] = 0x8B;
	buf->Name[14] = 0xCD;
	buf->Name[15] = 0x7C;
	buf->Mode = cpu_to_le32(mode);
	cifs_dbg(FYI, "mode on posix create 0%o", mode);
	return buf;
}