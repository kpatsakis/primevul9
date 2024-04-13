long keyctl_instantiate_key(key_serial_t id,
			    const void __user *_payload,
			    size_t plen,
			    key_serial_t ringid)
{
	if (_payload && plen) {
		struct iovec iov;
		struct iov_iter from;
		int ret;

		ret = import_single_range(WRITE, (void __user *)_payload, plen,
					  &iov, &from);
		if (unlikely(ret))
			return ret;

		return keyctl_instantiate_key_common(id, &from, ringid);
	}

	return keyctl_instantiate_key_common(id, NULL, ringid);
}