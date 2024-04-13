smb2_validate_and_copy_iov(unsigned int offset, unsigned int buffer_length,
			   struct kvec *iov, unsigned int minbufsize,
			   char *data)
{
	char *begin_of_buf = offset + (char *)iov->iov_base;
	int rc;

	if (!data)
		return -EINVAL;

	rc = smb2_validate_iov(offset, buffer_length, iov, minbufsize);
	if (rc)
		return rc;

	memcpy(data, begin_of_buf, buffer_length);

	return 0;
}