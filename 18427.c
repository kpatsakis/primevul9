static void _get_random_bytes(void *buf, int nbytes)
{
	__u8 tmp[CHACHA_BLOCK_SIZE] __aligned(4);

	trace_get_random_bytes(nbytes, _RET_IP_);

	while (nbytes >= CHACHA_BLOCK_SIZE) {
		extract_crng(buf);
		buf += CHACHA_BLOCK_SIZE;
		nbytes -= CHACHA_BLOCK_SIZE;
	}

	if (nbytes > 0) {
		extract_crng(tmp);
		memcpy(buf, tmp, nbytes);
		crng_backtrack_protect(tmp, nbytes);
	} else
		crng_backtrack_protect(tmp, CHACHA_BLOCK_SIZE);
	memzero_explicit(tmp, sizeof(tmp));
}