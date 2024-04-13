SMB2_set_compression(const unsigned int xid, struct cifs_tcon *tcon,
		     u64 persistent_fid, u64 volatile_fid)
{
	int rc;
	struct  compress_ioctl fsctl_input;
	char *ret_data = NULL;

	fsctl_input.CompressionState =
			cpu_to_le16(COMPRESSION_FORMAT_DEFAULT);

	rc = SMB2_ioctl(xid, tcon, persistent_fid, volatile_fid,
			FSCTL_SET_COMPRESSION, true /* is_fsctl */,
			(char *)&fsctl_input /* data input */,
			2 /* in data len */, CIFSMaxBufSize /* max out data */,
			&ret_data /* out data */, NULL);

	cifs_dbg(FYI, "set compression rc %d\n", rc);

	return rc;
}