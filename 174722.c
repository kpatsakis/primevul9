static int wrap_nettle_hash_exists(gnutls_digest_algorithm_t algo)
{
	switch (algo) {
	case GNUTLS_DIG_MD5:
	case GNUTLS_DIG_SHA1:
	case GNUTLS_DIG_MD5_SHA1:

	case GNUTLS_DIG_SHA224:
	case GNUTLS_DIG_SHA256:
	case GNUTLS_DIG_SHA384:
	case GNUTLS_DIG_SHA512:

#ifdef NETTLE_SHA3_FIPS202
	case GNUTLS_DIG_SHA3_224:
	case GNUTLS_DIG_SHA3_256:
	case GNUTLS_DIG_SHA3_384:
	case GNUTLS_DIG_SHA3_512:
#endif

	case GNUTLS_DIG_SHAKE_128:
	case GNUTLS_DIG_SHAKE_256:

	case GNUTLS_DIG_MD2:
	case GNUTLS_DIG_RMD160:

#if ENABLE_GOST
	case GNUTLS_DIG_GOSTR_94:
	case GNUTLS_DIG_STREEBOG_256:
	case GNUTLS_DIG_STREEBOG_512:
#endif
		return 1;
	default:
		return 0;
	}
}