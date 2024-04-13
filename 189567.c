compute_cookie(ns_client_t *client, uint32_t when, uint32_t nonce,
	       const unsigned char *secret, isc_buffer_t *buf)
{
	switch (ns_g_server->cookiealg) {
#if defined(HAVE_OPENSSL_AES) || defined(HAVE_OPENSSL_EVP_AES)
	case ns_cookiealg_aes: {
		unsigned char digest[ISC_AES_BLOCK_LENGTH];
		unsigned char input[4 + 4 + 16];
		isc_netaddr_t netaddr;
		unsigned char *cp;
		unsigned int i;

		memset(input, 0, sizeof(input));
		cp = isc_buffer_used(buf);
		isc_buffer_putmem(buf, client->cookie, 8);
		isc_buffer_putuint32(buf, nonce);
		isc_buffer_putuint32(buf, when);
		memmove(input, cp, 16);
		isc_aes128_crypt(secret, input, digest);
		for (i = 0; i < 8; i++)
			input[i] = digest[i] ^ digest[i + 8];
		isc_netaddr_fromsockaddr(&netaddr, &client->peeraddr);
		switch (netaddr.family) {
		case AF_INET:
			cp = (unsigned char *)&netaddr.type.in;
			memmove(input + 8, cp, 4);
			memset(input + 12, 0, 4);
			isc_aes128_crypt(secret, input, digest);
			break;
		case AF_INET6:
			cp = (unsigned char *)&netaddr.type.in6;
			memmove(input + 8, cp, 16);
			isc_aes128_crypt(secret, input, digest);
			for (i = 0; i < 8; i++)
				input[i + 8] = digest[i] ^ digest[i + 8];
			isc_aes128_crypt(ns_g_server->secret, input + 8,
					 digest);
			break;
		}
		for (i = 0; i < 8; i++)
			digest[i] ^= digest[i + 8];
		isc_buffer_putmem(buf, digest, 8);
		break;
	}
#endif

	case ns_cookiealg_sha1: {
		unsigned char digest[ISC_SHA1_DIGESTLENGTH];
		isc_netaddr_t netaddr;
		unsigned char *cp;
		isc_hmacsha1_t hmacsha1;
		unsigned int length;

		cp = isc_buffer_used(buf);
		isc_buffer_putmem(buf, client->cookie, 8);
		isc_buffer_putuint32(buf, nonce);
		isc_buffer_putuint32(buf, when);

		isc_hmacsha1_init(&hmacsha1, secret, ISC_SHA1_DIGESTLENGTH);
		isc_hmacsha1_update(&hmacsha1, cp, 16);
		isc_netaddr_fromsockaddr(&netaddr, &client->peeraddr);
		switch (netaddr.family) {
		case AF_INET:
			cp = (unsigned char *)&netaddr.type.in;
			length = 4;
			break;
		case AF_INET6:
			cp = (unsigned char *)&netaddr.type.in6;
			length = 16;
			break;
		default:
			INSIST(0);
			ISC_UNREACHABLE();
		}
		isc_hmacsha1_update(&hmacsha1, cp, length);
		isc_hmacsha1_sign(&hmacsha1, digest, sizeof(digest));
		isc_buffer_putmem(buf, digest, 8);
		isc_hmacsha1_invalidate(&hmacsha1);
		break;
	}

	case ns_cookiealg_sha256: {
		unsigned char digest[ISC_SHA256_DIGESTLENGTH];
		isc_netaddr_t netaddr;
		unsigned char *cp;
		isc_hmacsha256_t hmacsha256;
		unsigned int length;

		cp = isc_buffer_used(buf);
		isc_buffer_putmem(buf, client->cookie, 8);
		isc_buffer_putuint32(buf, nonce);
		isc_buffer_putuint32(buf, when);

		isc_hmacsha256_init(&hmacsha256, secret,
				    ISC_SHA256_DIGESTLENGTH);
		isc_hmacsha256_update(&hmacsha256, cp, 16);
		isc_netaddr_fromsockaddr(&netaddr, &client->peeraddr);
		switch (netaddr.family) {
		case AF_INET:
			cp = (unsigned char *)&netaddr.type.in;
			length = 4;
			break;
		case AF_INET6:
			cp = (unsigned char *)&netaddr.type.in6;
			length = 16;
			break;
		default:
			INSIST(0);
			ISC_UNREACHABLE();
		}
		isc_hmacsha256_update(&hmacsha256, cp, length);
		isc_hmacsha256_sign(&hmacsha256, digest, sizeof(digest));
		isc_buffer_putmem(buf, digest, 8);
		isc_hmacsha256_invalidate(&hmacsha256);
		break;
	}
	default:
		INSIST(0);
		ISC_UNREACHABLE();
	}
}