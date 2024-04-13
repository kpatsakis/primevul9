static void gen_challenge(const char *addr, char *challenge)
{
	char input[32];
	char digest[MAX_DIGEST_LEN];
	struct timeval tv;
	int len;

	memset(input, 0, sizeof input);

	strlcpy(input, addr, 17);
	sys_gettimeofday(&tv);
	SIVAL(input, 16, tv.tv_sec);
	SIVAL(input, 20, tv.tv_usec);
	SIVAL(input, 24, getpid());

	sum_init(-1, 0);
	sum_update(input, sizeof input);
	len = sum_end(digest);

	base64_encode(digest, len, challenge, 0);
}