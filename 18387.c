static void _mix_pool_bytes(struct entropy_store *r, const void *in,
			    int nbytes)
{
	unsigned long i, tap1, tap2, tap3, tap4, tap5;
	int input_rotate;
	int wordmask = r->poolinfo->poolwords - 1;
	const char *bytes = in;
	__u32 w;

	tap1 = r->poolinfo->tap1;
	tap2 = r->poolinfo->tap2;
	tap3 = r->poolinfo->tap3;
	tap4 = r->poolinfo->tap4;
	tap5 = r->poolinfo->tap5;

	input_rotate = r->input_rotate;
	i = r->add_ptr;

	/* mix one byte at a time to simplify size handling and churn faster */
	while (nbytes--) {
		w = rol32(*bytes++, input_rotate);
		i = (i - 1) & wordmask;

		/* XOR in the various taps */
		w ^= r->pool[i];
		w ^= r->pool[(i + tap1) & wordmask];
		w ^= r->pool[(i + tap2) & wordmask];
		w ^= r->pool[(i + tap3) & wordmask];
		w ^= r->pool[(i + tap4) & wordmask];
		w ^= r->pool[(i + tap5) & wordmask];

		/* Mix the result back in with a twist */
		r->pool[i] = (w >> 3) ^ twist_table[w & 7];

		/*
		 * Normally, we add 7 bits of rotation to the pool.
		 * At the beginning of the pool, add an extra 7 bits
		 * rotation, so that successive passes spread the
		 * input bits across the pool evenly.
		 */
		input_rotate = (input_rotate + (i ? 7 : 14)) & 31;
	}

	r->input_rotate = input_rotate;
	r->add_ptr = i;
}