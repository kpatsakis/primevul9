winzip_aes_new(int mode, const char *password) {
    struct winzip_aes *ctx;
    
    if ((ctx = (struct winzip_aes *)malloc(sizeof(*ctx))) == NULL) {
	return NULL;
    }
    
    if ((ctx->password = strdup(password)) == NULL) {
	free(ctx);
	return NULL;
    }

    ctx->mode = mode;

    zip_error_init(&ctx->error);

    return ctx;
}