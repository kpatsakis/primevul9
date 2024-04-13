static void generate_hash(const char *in, const char *challenge, char *out)
{
	char buf[MAX_DIGEST_LEN];
	int len;

	sum_init(-1, 0);
	sum_update(in, strlen(in));
	sum_update(challenge, strlen(challenge));
	len = sum_end(buf);

	base64_encode(buf, len, out, 0);
}