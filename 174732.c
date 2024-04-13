static void _md5_sha1_digest(void *_ctx, size_t len, uint8_t *digest)
{
	struct md5_sha1_ctx *ctx = _ctx;

	md5_digest(&ctx->md5, len <= MD5_DIGEST_SIZE ? len : MD5_DIGEST_SIZE,
		   digest);

	if (len > MD5_DIGEST_SIZE)
		sha1_digest(&ctx->sha1, len - MD5_DIGEST_SIZE,
			    digest + MD5_DIGEST_SIZE);
}