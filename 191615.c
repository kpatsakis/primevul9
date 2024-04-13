zip_source_winzip_aes(zip_t *za, zip_source_t *src, zip_uint16_t em, int flags, const char *password)
{
    zip_source_t *s2;
    int mode = 0;
    zip_stat_t st;
    zip_uint64_t aux_length;
    struct winzip_aes *ctx;

    switch (em) {
    case ZIP_EM_AES_128:
	mode = 1;
	break;
    case ZIP_EM_AES_192:
	mode = 2;
	break;
    case ZIP_EM_AES_256:
	mode = 3;
	break;
    }
	
    if (password == NULL || src == NULL || mode == 0) {
	zip_error_set(&za->error, ZIP_ER_INVAL, 0);
	return NULL;
    }
    if (flags & ZIP_CODEC_ENCODE) {
	zip_error_set(&za->error, ZIP_ER_ENCRNOTSUPP, 0);
	return NULL;
    }

    if (zip_source_stat(src, &st) != 0) {
	_zip_error_set_from_source(&za->error, src);
	return NULL;
    }

    aux_length = PWD_VER_LENGTH + salt_length[mode] + HMAC_LENGTH;
    
    if ((st.valid & ZIP_STAT_COMP_SIZE) == 0 || st.comp_size < aux_length) {
	zip_error_set(&za->error, ZIP_ER_OPNOTSUPP, 0);
	return NULL;
    }

    if ((ctx = winzip_aes_new(mode, password)) == NULL) {
	zip_error_set(&za->error, ZIP_ER_MEMORY, 0);
	return NULL;
    }

    ctx->data_length = st.comp_size - aux_length;

    if ((s2 = zip_source_layered(za, src, winzip_aes_decrypt, ctx)) == NULL) {
	winzip_aes_free(ctx);
	return NULL;
    }

    return s2;
}