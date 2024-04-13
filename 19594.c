static bool iwl_write_to_user_buf(char __user *user_buf, ssize_t count,
				  void *buf, ssize_t *size,
				  ssize_t *bytes_copied)
{
	int buf_size_left = count - *bytes_copied;

	buf_size_left = buf_size_left - (buf_size_left % sizeof(u32));
	if (*size > buf_size_left)
		*size = buf_size_left;

	*size -= copy_to_user(user_buf, buf, *size);
	*bytes_copied += *size;

	if (buf_size_left == *size)
		return true;
	return false;
}