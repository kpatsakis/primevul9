ldns_digest_function(char *name)
{
	/* these are the mandatory algorithms from RFC4635 */
	/* The optional algorithms are not yet implemented */
	if (strcasecmp(name, "hmac-sha512.") == 0) {
#ifdef HAVE_EVP_SHA512
		return EVP_sha512();
#else
		return NULL;
#endif
	} else if (strcasecmp(name, "hmac-shac384.") == 0) {
#ifdef HAVE_EVP_SHA384
		return EVP_sha384();
#else
		return NULL;
#endif
	} else if (strcasecmp(name, "hmac-sha256.") == 0) {
#ifdef HAVE_EVP_SHA256
		return EVP_sha256();
#else
		return NULL;
#endif
	} else if (strcasecmp(name, "hmac-sha1.") == 0) {
		return EVP_sha1();
	} else if (strcasecmp(name, "hmac-md5.sig-alg.reg.int.") == 0) {
		return EVP_md5();
	} else {
		return NULL;
	}
}