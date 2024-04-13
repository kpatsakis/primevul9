static int bpf_task_fd_query_copy(const union bpf_attr *attr,
				    union bpf_attr __user *uattr,
				    u32 prog_id, u32 fd_type,
				    const char *buf, u64 probe_offset,
				    u64 probe_addr)
{
	char __user *ubuf = u64_to_user_ptr(attr->task_fd_query.buf);
	u32 len = buf ? strlen(buf) : 0, input_len;
	int err = 0;

	if (put_user(len, &uattr->task_fd_query.buf_len))
		return -EFAULT;
	input_len = attr->task_fd_query.buf_len;
	if (input_len && ubuf) {
		if (!len) {
			/* nothing to copy, just make ubuf NULL terminated */
			char zero = '\0';

			if (put_user(zero, ubuf))
				return -EFAULT;
		} else if (input_len >= len + 1) {
			/* ubuf can hold the string with NULL terminator */
			if (copy_to_user(ubuf, buf, len + 1))
				return -EFAULT;
		} else {
			/* ubuf cannot hold the string with NULL terminator,
			 * do a partial copy with NULL terminator.
			 */
			char zero = '\0';

			err = -ENOSPC;
			if (copy_to_user(ubuf, buf, input_len - 1))
				return -EFAULT;
			if (put_user(zero, ubuf + input_len - 1))
				return -EFAULT;
		}
	}

	if (put_user(prog_id, &uattr->task_fd_query.prog_id) ||
	    put_user(fd_type, &uattr->task_fd_query.fd_type) ||
	    put_user(probe_offset, &uattr->task_fd_query.probe_offset) ||
	    put_user(probe_addr, &uattr->task_fd_query.probe_addr))
		return -EFAULT;

	return err;
}