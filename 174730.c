static int _mac_ctx_init(gnutls_mac_algorithm_t algo,
			 struct nettle_mac_ctx *ctx)
{
	/* Any FIPS140-2 related enforcement is performed on
	 * gnutls_hash_init() and gnutls_hmac_init() */

	ctx->set_nonce = NULL;
	switch (algo) {
	case GNUTLS_MAC_MD5:
		ctx->update = (update_func) hmac_md5_update;
		ctx->digest = (digest_func) hmac_md5_digest;
		ctx->set_key = (set_key_func) hmac_md5_set_key;
		ctx->ctx_ptr = &ctx->ctx.md5;
		ctx->length = MD5_DIGEST_SIZE;
		break;
	case GNUTLS_MAC_SHA1:
		ctx->update = (update_func) hmac_sha1_update;
		ctx->digest = (digest_func) hmac_sha1_digest;
		ctx->set_key = (set_key_func) hmac_sha1_set_key;
		ctx->ctx_ptr = &ctx->ctx.sha1;
		ctx->length = SHA1_DIGEST_SIZE;
		break;
	case GNUTLS_MAC_SHA224:
		ctx->update = (update_func) hmac_sha224_update;
		ctx->digest = (digest_func) hmac_sha224_digest;
		ctx->set_key = (set_key_func) hmac_sha224_set_key;
		ctx->ctx_ptr = &ctx->ctx.sha224;
		ctx->length = SHA224_DIGEST_SIZE;
		break;
	case GNUTLS_MAC_SHA256:
		ctx->update = (update_func) hmac_sha256_update;
		ctx->digest = (digest_func) hmac_sha256_digest;
		ctx->set_key = (set_key_func) hmac_sha256_set_key;
		ctx->ctx_ptr = &ctx->ctx.sha256;
		ctx->length = SHA256_DIGEST_SIZE;
		break;
	case GNUTLS_MAC_SHA384:
		ctx->update = (update_func) hmac_sha384_update;
		ctx->digest = (digest_func) hmac_sha384_digest;
		ctx->set_key = (set_key_func) hmac_sha384_set_key;
		ctx->ctx_ptr = &ctx->ctx.sha384;
		ctx->length = SHA384_DIGEST_SIZE;
		break;
	case GNUTLS_MAC_SHA512:
		ctx->update = (update_func) hmac_sha512_update;
		ctx->digest = (digest_func) hmac_sha512_digest;
		ctx->set_key = (set_key_func) hmac_sha512_set_key;
		ctx->ctx_ptr = &ctx->ctx.sha512;
		ctx->length = SHA512_DIGEST_SIZE;
		break;
#if ENABLE_GOST
	case GNUTLS_MAC_GOSTR_94:
		ctx->update = (update_func) hmac_gosthash94cp_update;
		ctx->digest = (digest_func) hmac_gosthash94cp_digest;
		ctx->set_key = (set_key_func) hmac_gosthash94cp_set_key;
		ctx->ctx_ptr = &ctx->ctx.gosthash94cp;
		ctx->length = GOSTHASH94CP_DIGEST_SIZE;
		break;
	case GNUTLS_MAC_STREEBOG_256:
		ctx->update = (update_func) hmac_streebog256_update;
		ctx->digest = (digest_func) hmac_streebog256_digest;
		ctx->set_key = (set_key_func) hmac_streebog256_set_key;
		ctx->ctx_ptr = &ctx->ctx.streebog256;
		ctx->length = STREEBOG256_DIGEST_SIZE;
		break;
	case GNUTLS_MAC_STREEBOG_512:
		ctx->update = (update_func) hmac_streebog512_update;
		ctx->digest = (digest_func) hmac_streebog512_digest;
		ctx->set_key = (set_key_func) hmac_streebog512_set_key;
		ctx->ctx_ptr = &ctx->ctx.streebog512;
		ctx->length = STREEBOG512_DIGEST_SIZE;
		break;
	case GNUTLS_MAC_GOST28147_TC26Z_IMIT:
		ctx->update = (update_func) gost28147_imit_update;
		ctx->digest = (digest_func) gost28147_imit_digest;
		ctx->set_key = _wrap_gost28147_imit_set_key_tc26z;
		ctx->ctx_ptr = &ctx->ctx.gost28147_imit;
		ctx->length = GOST28147_IMIT_DIGEST_SIZE;
		break;
	case GNUTLS_MAC_MAGMA_OMAC:
		ctx->update = (update_func) cmac_magma_update;
		ctx->digest = (digest_func) cmac_magma_digest;
		ctx->set_key = _wrap_cmac_magma_set_key;
		ctx->ctx_ptr = &ctx->ctx.magma;
		ctx->length = CMAC64_DIGEST_SIZE;
		break;
	case GNUTLS_MAC_KUZNYECHIK_OMAC:
		ctx->update = (update_func) cmac_kuznyechik_update;
		ctx->digest = (digest_func) cmac_kuznyechik_digest;
		ctx->set_key = _wrap_cmac_kuznyechik_set_key;
		ctx->ctx_ptr = &ctx->ctx.kuznyechik;
		ctx->length = CMAC128_DIGEST_SIZE;
		break;
#endif
	case GNUTLS_MAC_UMAC_96:
		ctx->update = (update_func) umac96_update;
		ctx->digest = (digest_func) umac96_digest;
		ctx->set_key = _wrap_umac96_set_key;
		ctx->set_nonce = (set_nonce_func) umac96_set_nonce;
		ctx->ctx_ptr = &ctx->ctx.umac96;
		ctx->length = 12;
		break;
	case GNUTLS_MAC_UMAC_128:
		ctx->update = (update_func) umac128_update;
		ctx->digest = (digest_func) umac128_digest;
		ctx->set_key = _wrap_umac128_set_key;
		ctx->set_nonce = (set_nonce_func) umac128_set_nonce;
		ctx->ctx_ptr = &ctx->ctx.umac128;
		ctx->length = 16;
		break;
	case GNUTLS_MAC_AES_CMAC_128:
		ctx->update = (update_func) cmac_aes128_update;
		ctx->digest = (digest_func) cmac_aes128_digest;
		ctx->set_key = _wrap_cmac128_set_key;
		ctx->ctx_ptr = &ctx->ctx.cmac128;
		ctx->length = CMAC128_DIGEST_SIZE;
		break;
	case GNUTLS_MAC_AES_CMAC_256:
		ctx->update = (update_func) cmac_aes256_update;
		ctx->digest = (digest_func) cmac_aes256_digest;
		ctx->set_key = _wrap_cmac256_set_key;
		ctx->ctx_ptr = &ctx->ctx.cmac256;
		ctx->length = CMAC128_DIGEST_SIZE;
		break;
	case GNUTLS_MAC_AES_GMAC_128:
		ctx->set_key = _wrap_gmac_aes128_set_key;
		ctx->set_nonce = _wrap_gmac_set_nonce;
		ctx->update = _wrap_gmac_update;
		ctx->digest = _wrap_gmac_digest;
		ctx->ctx_ptr = &ctx->ctx.gmac;
		ctx->length = GCM_DIGEST_SIZE;
		ctx->ctx.gmac.encrypt = (nettle_cipher_func *)aes128_encrypt;
		break;
	case GNUTLS_MAC_AES_GMAC_192:
		ctx->set_key = _wrap_gmac_aes192_set_key;
		ctx->set_nonce = _wrap_gmac_set_nonce;
		ctx->update = _wrap_gmac_update;
		ctx->digest = _wrap_gmac_digest;
		ctx->ctx_ptr = &ctx->ctx.gmac;
		ctx->length = GCM_DIGEST_SIZE;
		ctx->ctx.gmac.encrypt = (nettle_cipher_func *)aes192_encrypt;
		break;
	case GNUTLS_MAC_AES_GMAC_256:
		ctx->set_key = _wrap_gmac_aes256_set_key;
		ctx->set_nonce = _wrap_gmac_set_nonce;
		ctx->update = _wrap_gmac_update;
		ctx->digest = _wrap_gmac_digest;
		ctx->ctx_ptr = &ctx->ctx.gmac;
		ctx->length = GCM_DIGEST_SIZE;
		ctx->ctx.gmac.encrypt = (nettle_cipher_func *)aes256_encrypt;
		break;
	default:
		gnutls_assert();
		return GNUTLS_E_INVALID_REQUEST;
	}

	return 0;
}