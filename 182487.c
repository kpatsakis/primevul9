SMB2_set_eof(const unsigned int xid, struct cifs_tcon *tcon, u64 persistent_fid,
	     u64 volatile_fid, u32 pid, __le64 *eof)
{
	struct smb2_file_eof_info info;
	void *data;
	unsigned int size;

	info.EndOfFile = *eof;

	data = &info;
	size = sizeof(struct smb2_file_eof_info);

	return send_set_info(xid, tcon, persistent_fid, volatile_fid,
			pid, FILE_END_OF_FILE_INFORMATION, SMB2_O_INFO_FILE,
			0, 1, &data, &size);
}