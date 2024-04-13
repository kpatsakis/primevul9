winzip_aes_free(struct winzip_aes *ctx)
{
    if (ctx == NULL) {
	return;
    }

    _zip_crypto_clear(&ctx->fcrypt_ctx, sizeof(ctx->fcrypt_ctx));
    _zip_crypto_clear(ctx->password, strlen(ctx->password));
    free(ctx->password);
    zip_error_fini(&ctx->error);
    free(ctx);
}