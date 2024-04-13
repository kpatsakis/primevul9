wrap_nettle_mac_set_nonce(void *_ctx, const void *nonce, size_t noncelen)
{
	struct nettle_mac_ctx *ctx = _ctx;

	if (ctx->set_nonce == NULL)
		return gnutls_assert_val(GNUTLS_E_INVALID_REQUEST);

	if (nonce == NULL || noncelen == 0)
		return gnutls_assert_val(GNUTLS_E_INVALID_REQUEST);

	ctx->set_nonce(ctx->ctx_ptr, noncelen, nonce);

	return GNUTLS_E_SUCCESS;
}