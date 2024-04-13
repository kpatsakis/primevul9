wrap_nettle_mac_output(void *src_ctx, void *digest, size_t digestsize)
{
	struct nettle_mac_ctx *ctx;
	ctx = src_ctx;

	if (digestsize < ctx->length) {
		gnutls_assert();
		return GNUTLS_E_SHORT_MEMORY_BUFFER;
	}

	ctx->digest(ctx->ctx_ptr, digestsize, digest);

	return 0;
}