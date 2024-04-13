int __must_check get_random_bytes_arch(void *buf, int nbytes)
{
	int left = nbytes;
	char *p = buf;

	trace_get_random_bytes_arch(left, _RET_IP_);
	while (left) {
		unsigned long v;
		int chunk = min_t(int, left, sizeof(unsigned long));

		if (!arch_get_random_long(&v))
			break;

		memcpy(p, &v, chunk);
		p += chunk;
		left -= chunk;
	}

	return nbytes - left;
}