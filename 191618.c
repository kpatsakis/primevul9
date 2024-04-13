verify_hmac(zip_source_t *src, struct winzip_aes *ctx)
{
    unsigned char computed[HMAC_LENGTH], from_file[HMAC_LENGTH];
    if (zip_source_read(src, from_file, HMAC_LENGTH) < HMAC_LENGTH) {
	_zip_error_set_from_source(&ctx->error, src);
	return false;
    }

    _zip_fcrypt_end(computed, &ctx->fcrypt_ctx);
    
    if (memcmp(from_file, computed, HMAC_LENGTH) != 0) {
	zip_error_set(&ctx->error, ZIP_ER_CRC, 0);
	return false;
    }

    return true;
}