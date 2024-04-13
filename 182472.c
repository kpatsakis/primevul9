smb2_validate_iov(unsigned int offset, unsigned int buffer_length,
		  struct kvec *iov, unsigned int min_buf_size)
{
	unsigned int smb_len = iov->iov_len;
	char *end_of_smb = smb_len + (char *)iov->iov_base;
	char *begin_of_buf = offset + (char *)iov->iov_base;
	char *end_of_buf = begin_of_buf + buffer_length;


	if (buffer_length < min_buf_size) {
		cifs_dbg(VFS, "buffer length %d smaller than minimum size %d\n",
			 buffer_length, min_buf_size);
		return -EINVAL;
	}

	/* check if beyond RFC1001 maximum length */
	if ((smb_len > 0x7FFFFF) || (buffer_length > 0x7FFFFF)) {
		cifs_dbg(VFS, "buffer length %d or smb length %d too large\n",
			 buffer_length, smb_len);
		return -EINVAL;
	}

	if ((begin_of_buf > end_of_smb) || (end_of_buf > end_of_smb)) {
		cifs_dbg(VFS, "illegal server response, bad offset to data\n");
		return -EINVAL;
	}

	return 0;
}