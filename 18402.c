static void extract_buf(struct entropy_store *r, __u8 *out)
{
	int i;
	union {
		__u32 w[5];
		unsigned long l[LONGS(20)];
	} hash;
	__u32 workspace[SHA1_WORKSPACE_WORDS];
	unsigned long flags;

	/*
	 * If we have an architectural hardware random number
	 * generator, use it for SHA's initial vector
	 */
	sha1_init(hash.w);
	for (i = 0; i < LONGS(20); i++) {
		unsigned long v;
		if (!arch_get_random_long(&v))
			break;
		hash.l[i] = v;
	}

	/* Generate a hash across the pool, 16 words (512 bits) at a time */
	spin_lock_irqsave(&r->lock, flags);
	for (i = 0; i < r->poolinfo->poolwords; i += 16)
		sha1_transform(hash.w, (__u8 *)(r->pool + i), workspace);

	/*
	 * We mix the hash back into the pool to prevent backtracking
	 * attacks (where the attacker knows the state of the pool
	 * plus the current outputs, and attempts to find previous
	 * ouputs), unless the hash function can be inverted. By
	 * mixing at least a SHA1 worth of hash data back, we make
	 * brute-forcing the feedback as hard as brute-forcing the
	 * hash.
	 */
	__mix_pool_bytes(r, hash.w, sizeof(hash.w));
	spin_unlock_irqrestore(&r->lock, flags);

	memzero_explicit(workspace, sizeof(workspace));

	/*
	 * In case the hash function has some recognizable output
	 * pattern, we fold it in half. Thus, we always feed back
	 * twice as much data as we output.
	 */
	hash.w[0] ^= hash.w[3];
	hash.w[1] ^= hash.w[4];
	hash.w[2] ^= rol32(hash.w[2], 16);

	memcpy(out, &hash, EXTRACT_SIZE);
	memzero_explicit(&hash, sizeof(hash));
}