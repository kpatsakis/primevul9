long keyctl_instantiate_key_iov(key_serial_t id,
				const struct iovec __user *_payload_iov,
				unsigned ioc,
				key_serial_t ringid)
{
	struct iovec iovstack[UIO_FASTIOV], *iov = iovstack;
	struct iov_iter from;
	long ret;

	if (!_payload_iov)
		ioc = 0;

	ret = import_iovec(WRITE, _payload_iov, ioc,
				    ARRAY_SIZE(iovstack), &iov, &from);
	if (ret < 0)
		return ret;
	ret = keyctl_instantiate_key_common(id, &from, ringid);
	kfree(iov);
	return ret;
}