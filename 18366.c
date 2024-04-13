static ssize_t extract_entropy(struct entropy_store *r, void *buf,
				 size_t nbytes, int min, int reserved)
{
	__u8 tmp[EXTRACT_SIZE];
	unsigned long flags;

	/* if last_data isn't primed, we need EXTRACT_SIZE extra bytes */
	if (fips_enabled) {
		spin_lock_irqsave(&r->lock, flags);
		if (!r->last_data_init) {
			r->last_data_init = 1;
			spin_unlock_irqrestore(&r->lock, flags);
			trace_extract_entropy(r->name, EXTRACT_SIZE,
					      ENTROPY_BITS(r), _RET_IP_);
			extract_buf(r, tmp);
			spin_lock_irqsave(&r->lock, flags);
			memcpy(r->last_data, tmp, EXTRACT_SIZE);
		}
		spin_unlock_irqrestore(&r->lock, flags);
	}

	trace_extract_entropy(r->name, nbytes, ENTROPY_BITS(r), _RET_IP_);
	nbytes = account(r, nbytes, min, reserved);

	return _extract_entropy(r, buf, nbytes, fips_enabled);
}