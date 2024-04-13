winzip_aes_decrypt(zip_source_t *src, void *ud, void *data, zip_uint64_t len, zip_source_cmd_t cmd)
{
    struct winzip_aes *ctx;
    zip_int64_t n;

    ctx = (struct winzip_aes *)ud;

    switch (cmd) {
    case ZIP_SOURCE_OPEN:
	if (decrypt_header(src, ctx) < 0) {
	    return -1;
	}
	ctx->current_position = 0;
	return 0;

    case ZIP_SOURCE_READ:
	if (len > ctx->data_length - ctx->current_position) {
	    len = ctx->data_length - ctx->current_position;
	}

	if (len == 0) {
	    if (!verify_hmac(src, ctx)) {
		return -1;
	    }
	    return 0;
	}
	
	if ((n=zip_source_read(src, data, len)) < 0) {
	    _zip_error_set_from_source(&ctx->error, src);
	    return -1;
	}
	ctx->current_position += n;

	/* TODO: loop if len > UINT_MAX */
	_zip_fcrypt_decrypt(data, n, &ctx->fcrypt_ctx);

	return n;

    case ZIP_SOURCE_CLOSE:
	return 0;

    case ZIP_SOURCE_STAT:
    {
	zip_stat_t *st;

	st = (zip_stat_t *)data;

	st->encryption_method = ZIP_EM_NONE;
	st->valid |= ZIP_STAT_ENCRYPTION_METHOD;
	if (st->valid & ZIP_STAT_COMP_SIZE) {
	    st->comp_size -= 12 + salt_length[ctx->mode];
	}
	
	return 0;
    }
            
    case ZIP_SOURCE_SUPPORTS:
	return zip_source_make_command_bitmap(ZIP_SOURCE_OPEN, ZIP_SOURCE_READ, ZIP_SOURCE_CLOSE, ZIP_SOURCE_STAT, ZIP_SOURCE_ERROR, ZIP_SOURCE_FREE, -1);

    case ZIP_SOURCE_ERROR:
	return zip_error_to_data(&ctx->error, data, len);

    case ZIP_SOURCE_FREE:
	winzip_aes_free(ctx);
	return 0;

    default:
	zip_error_set(&ctx->error, ZIP_ER_INVAL, 0);
	return -1;
    }
}