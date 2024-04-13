static int _ctx_init(gnutls_digest_algorithm_t algo,
		     struct nettle_hash_ctx *ctx)
{
	/* Any FIPS140-2 related enforcement is performed on
	 * gnutls_hash_init() and gnutls_hmac_init() */
	switch (algo) {
	case GNUTLS_DIG_MD5:
		md5_init(&ctx->ctx.md5);
		ctx->update = (update_func) md5_update;
		ctx->digest = (digest_func) md5_digest;
		ctx->ctx_ptr = &ctx->ctx.md5;
		ctx->length = MD5_DIGEST_SIZE;
		break;
	case GNUTLS_DIG_SHA1:
		sha1_init(&ctx->ctx.sha1);
		ctx->update = (update_func) sha1_update;
		ctx->digest = (digest_func) sha1_digest;
		ctx->ctx_ptr = &ctx->ctx.sha1;
		ctx->length = SHA1_DIGEST_SIZE;
		break;
	case GNUTLS_DIG_MD5_SHA1:
		md5_init(&ctx->ctx.md5_sha1.md5);
		sha1_init(&ctx->ctx.md5_sha1.sha1);
		ctx->update = (update_func) _md5_sha1_update;
		ctx->digest = (digest_func) _md5_sha1_digest;
		ctx->ctx_ptr = &ctx->ctx.md5_sha1;
		ctx->length = MD5_DIGEST_SIZE + SHA1_DIGEST_SIZE;
		break;
	case GNUTLS_DIG_SHA224:
		sha224_init(&ctx->ctx.sha224);
		ctx->update = (update_func) sha224_update;
		ctx->digest = (digest_func) sha224_digest;
		ctx->ctx_ptr = &ctx->ctx.sha224;
		ctx->length = SHA224_DIGEST_SIZE;
		break;
	case GNUTLS_DIG_SHA256:
		sha256_init(&ctx->ctx.sha256);
		ctx->update = (update_func) sha256_update;
		ctx->digest = (digest_func) sha256_digest;
		ctx->ctx_ptr = &ctx->ctx.sha256;
		ctx->length = SHA256_DIGEST_SIZE;
		break;
	case GNUTLS_DIG_SHA384:
		sha384_init(&ctx->ctx.sha384);
		ctx->update = (update_func) sha384_update;
		ctx->digest = (digest_func) sha384_digest;
		ctx->ctx_ptr = &ctx->ctx.sha384;
		ctx->length = SHA384_DIGEST_SIZE;
		break;
	case GNUTLS_DIG_SHA512:
		sha512_init(&ctx->ctx.sha512);
		ctx->update = (update_func) sha512_update;
		ctx->digest = (digest_func) sha512_digest;
		ctx->ctx_ptr = &ctx->ctx.sha512;
		ctx->length = SHA512_DIGEST_SIZE;
		break;
#ifdef NETTLE_SHA3_FIPS202
	case GNUTLS_DIG_SHA3_224:
		sha3_224_init(&ctx->ctx.sha3_224);
		ctx->update = (update_func) sha3_224_update;
		ctx->digest = (digest_func) sha3_224_digest;
		ctx->ctx_ptr = &ctx->ctx.sha3_224;
		ctx->length = SHA3_224_DIGEST_SIZE;
		break;
	case GNUTLS_DIG_SHA3_256:
		sha3_256_init(&ctx->ctx.sha3_256);
		ctx->update = (update_func) sha3_256_update;
		ctx->digest = (digest_func) sha3_256_digest;
		ctx->ctx_ptr = &ctx->ctx.sha3_256;
		ctx->length = SHA3_256_DIGEST_SIZE;
		break;
	case GNUTLS_DIG_SHA3_384:
		sha3_384_init(&ctx->ctx.sha3_384);
		ctx->update = (update_func) sha3_384_update;
		ctx->digest = (digest_func) sha3_384_digest;
		ctx->ctx_ptr = &ctx->ctx.sha3_384;
		ctx->length = SHA3_384_DIGEST_SIZE;
		break;
	case GNUTLS_DIG_SHA3_512:
		sha3_512_init(&ctx->ctx.sha3_512);
		ctx->update = (update_func) sha3_512_update;
		ctx->digest = (digest_func) sha3_512_digest;
		ctx->ctx_ptr = &ctx->ctx.sha3_512;
		ctx->length = SHA3_512_DIGEST_SIZE;
		break;
#endif
	case GNUTLS_DIG_MD2:
		md2_init(&ctx->ctx.md2);
		ctx->update = (update_func) md2_update;
		ctx->digest = (digest_func) md2_digest;
		ctx->ctx_ptr = &ctx->ctx.md2;
		ctx->length = MD2_DIGEST_SIZE;
		break;
#if ENABLE_GOST
	case GNUTLS_DIG_GOSTR_94:
		gosthash94cp_init(&ctx->ctx.gosthash94cp);
		ctx->update = (update_func) gosthash94cp_update;
		ctx->digest = (digest_func) gosthash94cp_digest;
		ctx->ctx_ptr = &ctx->ctx.gosthash94cp;
		ctx->length = GOSTHASH94_DIGEST_SIZE;
		break;
	case GNUTLS_DIG_STREEBOG_256:
		streebog256_init(&ctx->ctx.streebog256);
		ctx->update = (update_func) streebog256_update;
		ctx->digest = (digest_func) streebog256_digest;
		ctx->ctx_ptr = &ctx->ctx.streebog256;
		ctx->length = STREEBOG256_DIGEST_SIZE;
		break;
	case GNUTLS_DIG_STREEBOG_512:
		streebog512_init(&ctx->ctx.streebog512);
		ctx->update = (update_func) streebog512_update;
		ctx->digest = (digest_func) streebog512_digest;
		ctx->ctx_ptr = &ctx->ctx.streebog512;
		ctx->length = STREEBOG512_DIGEST_SIZE;
		break;
#endif
	default:
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	return 0;
}