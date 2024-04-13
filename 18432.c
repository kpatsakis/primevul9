static size_t account(struct entropy_store *r, size_t nbytes, int min,
		      int reserved)
{
	int entropy_count, orig, have_bytes;
	size_t ibytes, nfrac;

	BUG_ON(r->entropy_count > r->poolinfo->poolfracbits);

	/* Can we pull enough? */
retry:
	entropy_count = orig = READ_ONCE(r->entropy_count);
	ibytes = nbytes;
	/* never pull more than available */
	have_bytes = entropy_count >> (ENTROPY_SHIFT + 3);

	if ((have_bytes -= reserved) < 0)
		have_bytes = 0;
	ibytes = min_t(size_t, ibytes, have_bytes);
	if (ibytes < min)
		ibytes = 0;

	if (WARN_ON(entropy_count < 0)) {
		pr_warn("negative entropy count: pool %s count %d\n",
			r->name, entropy_count);
		entropy_count = 0;
	}
	nfrac = ibytes << (ENTROPY_SHIFT + 3);
	if ((size_t) entropy_count > nfrac)
		entropy_count -= nfrac;
	else
		entropy_count = 0;

	if (cmpxchg(&r->entropy_count, orig, entropy_count) != orig)
		goto retry;

	trace_debit_entropy(r->name, 8 * ibytes);
	if (ibytes && ENTROPY_BITS(r) < random_write_wakeup_bits) {
		wake_up_interruptible(&random_write_wait);
		kill_fasync(&fasync, SIGIO, POLL_OUT);
	}

	return ibytes;
}