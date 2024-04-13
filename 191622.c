decrypt_header(zip_source_t *src, struct winzip_aes *ctx)
{
    zip_uint8_t header[MAX_HEADER_LENGTH];
    zip_uint8_t password_verification[PWD_VER_LENGTH];
    zip_uint8_t headerlen;
    zip_int64_t n;
    unsigned char key[16];

    headerlen = PWD_VER_LENGTH + salt_length[ctx->mode];
    if ((n=zip_source_read(src, header, headerlen)) < 0) {
	_zip_error_set_from_source(&ctx->error, src);
	return -1;
    }
    
    if (n != headerlen) {
	zip_error_set(&ctx->error, ZIP_ER_EOF, 0);
	return -1;
    }

    if (_zip_fcrypt_init(ctx->mode, ctx->password, strlen(ctx->password), header, password_verification, &ctx->fcrypt_ctx) != 0) {
	zip_error_set(&ctx->error, ZIP_ER_MEMORY, 0);
	return -1;
    }
    if (memcmp(password_verification, header + salt_length[ctx->mode], PWD_VER_LENGTH) != 0) {
	zip_error_set(&ctx->error, ZIP_ER_WRONGPASSWD, 0);
	return -1;
    }
    return 0;
}