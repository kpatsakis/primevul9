static void credit_entropy_bits(struct entropy_store *r, int nbits)
{
	int entropy_count, orig, has_initialized = 0;
	const int pool_size = r->poolinfo->poolfracbits;
	int nfrac = nbits << ENTROPY_SHIFT;

	if (!nbits)
		return;

retry:
	entropy_count = orig = READ_ONCE(r->entropy_count);
	if (nfrac < 0) {
		/* Debit */
		entropy_count += nfrac;
	} else {
		/*
		 * Credit: we have to account for the possibility of
		 * overwriting already present entropy.	 Even in the
		 * ideal case of pure Shannon entropy, new contributions
		 * approach the full value asymptotically:
		 *
		 * entropy <- entropy + (pool_size - entropy) *
		 *	(1 - exp(-add_entropy/pool_size))
		 *
		 * For add_entropy <= pool_size/2 then
		 * (1 - exp(-add_entropy/pool_size)) >=
		 *    (add_entropy/pool_size)*0.7869...
		 * so we can approximate the exponential with
		 * 3/4*add_entropy/pool_size and still be on the
		 * safe side by adding at most pool_size/2 at a time.
		 *
		 * The use of pool_size-2 in the while statement is to
		 * prevent rounding artifacts from making the loop
		 * arbitrarily long; this limits the loop to log2(pool_size)*2
		 * turns no matter how large nbits is.
		 */
		int pnfrac = nfrac;
		const int s = r->poolinfo->poolbitshift + ENTROPY_SHIFT + 2;
		/* The +2 corresponds to the /4 in the denominator */

		do {
			unsigned int anfrac = min(pnfrac, pool_size/2);
			unsigned int add =
				((pool_size - entropy_count)*anfrac*3) >> s;

			entropy_count += add;
			pnfrac -= anfrac;
		} while (unlikely(entropy_count < pool_size-2 && pnfrac));
	}

	if (WARN_ON(entropy_count < 0)) {
		pr_warn("negative entropy/overflow: pool %s count %d\n",
			r->name, entropy_count);
		entropy_count = 0;
	} else if (entropy_count > pool_size)
		entropy_count = pool_size;
	if (cmpxchg(&r->entropy_count, orig, entropy_count) != orig)
		goto retry;

	if (has_initialized) {
		r->initialized = 1;
		kill_fasync(&fasync, SIGIO, POLL_IN);
	}

	trace_credit_entropy_bits(r->name, nbits,
				  entropy_count >> ENTROPY_SHIFT, _RET_IP_);

	if (r == &input_pool) {
		int entropy_bits = entropy_count >> ENTROPY_SHIFT;

		if (crng_init < 2) {
			if (entropy_bits < 128)
				return;
			crng_reseed(&primary_crng, r);
			entropy_bits = ENTROPY_BITS(r);
		}
	}
}