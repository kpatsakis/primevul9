static ssize_t _extract_entropy(struct entropy_store *r, void *buf,
				size_t nbytes, int fips)
{
	ssize_t ret = 0, i;
	__u8 tmp[EXTRACT_SIZE];
	unsigned long flags;

	while (nbytes) {
		extract_buf(r, tmp);

		if (fips) {
			spin_lock_irqsave(&r->lock, flags);
			if (!memcmp(tmp, r->last_data, EXTRACT_SIZE))
				panic("Hardware RNG duplicated output!\n");
			memcpy(r->last_data, tmp, EXTRACT_SIZE);
			spin_unlock_irqrestore(&r->lock, flags);
		}
		i = min_t(int, nbytes, EXTRACT_SIZE);
		memcpy(buf, tmp, i);
		nbytes -= i;
		buf += i;
		ret += i;
	}

	/* Wipe data just returned from memory */
	memzero_explicit(tmp, sizeof(tmp));

	return ret;
}