sc_pkcs15_convert_prkey(struct sc_pkcs15_prkey *pkcs15_key, void *evp_key)
{
#ifdef ENABLE_OPENSSL
	EVP_PKEY *pk = (EVP_PKEY *)evp_key;
	int pk_type;
	 pk_type = EVP_PKEY_base_id(pk);

	switch (pk_type) {
	case EVP_PKEY_RSA: {
		struct sc_pkcs15_prkey_rsa *dst = &pkcs15_key->u.rsa;
		RSA *src = EVP_PKEY_get1_RSA(pk);
		const BIGNUM *src_n, *src_e, *src_d, *src_p, *src_q, *src_iqmp, *src_dmp1, *src_dmq1;

		RSA_get0_key(src, &src_n, &src_e, &src_d);
		RSA_get0_factors(src, &src_p, &src_q);
		RSA_get0_crt_params(src, &src_dmp1, &src_dmq1, &src_iqmp);

		pkcs15_key->algorithm = SC_ALGORITHM_RSA;
		if (!sc_pkcs15_convert_bignum(&dst->modulus, src_n)
		 || !sc_pkcs15_convert_bignum(&dst->exponent, src_e)
		 || !sc_pkcs15_convert_bignum(&dst->d, src_d)
		 || !sc_pkcs15_convert_bignum(&dst->p, src_p)
		 || !sc_pkcs15_convert_bignum(&dst->q, src_q))
			return SC_ERROR_NOT_SUPPORTED;
		if (src_iqmp && src_dmp1 && src_dmq1) {
			sc_pkcs15_convert_bignum(&dst->iqmp, src_iqmp);
			sc_pkcs15_convert_bignum(&dst->dmp1, src_dmp1);
			sc_pkcs15_convert_bignum(&dst->dmq1, src_dmq1);
		}
		RSA_free(src);
		break;
		}
	case EVP_PKEY_DSA: {
		struct sc_pkcs15_prkey_dsa *dst = &pkcs15_key->u.dsa;
		DSA *src = EVP_PKEY_get1_DSA(pk);
		const BIGNUM *src_pub_key, *src_p, *src_q, *src_g, *src_priv_key;

		DSA_get0_key(src, &src_pub_key, &src_priv_key);
		DSA_get0_pqg(src, &src_p, &src_q, &src_g);

		pkcs15_key->algorithm = SC_ALGORITHM_DSA;
		sc_pkcs15_convert_bignum(&dst->pub, src_pub_key);
		sc_pkcs15_convert_bignum(&dst->p, src_p);
		sc_pkcs15_convert_bignum(&dst->q, src_q);
		sc_pkcs15_convert_bignum(&dst->g, src_g);
		sc_pkcs15_convert_bignum(&dst->priv, src_priv_key);
		DSA_free(src);
		break;
		}
#if OPENSSL_VERSION_NUMBER >= 0x10000000L && !defined(OPENSSL_NO_EC)
	case NID_id_GostR3410_2001: {
		struct sc_pkcs15_prkey_gostr3410 *dst = &pkcs15_key->u.gostr3410;
		EC_KEY *src = EVP_PKEY_get0(pk);

		assert(src);
		pkcs15_key->algorithm = SC_ALGORITHM_GOSTR3410;
		assert(EC_KEY_get0_private_key(src));
		sc_pkcs15_convert_bignum(&dst->d, EC_KEY_get0_private_key(src));
		break;
		}
	case EVP_PKEY_EC: {
		struct sc_pkcs15_prkey_ec *dst = &pkcs15_key->u.ec;
		EC_KEY *src = NULL;
		const EC_GROUP *grp = NULL;
		unsigned char buf[255];
		size_t buflen = 255;
		int nid;

		src = EVP_PKEY_get0(pk);
		assert(src);
		assert(EC_KEY_get0_private_key(src));
		assert(EC_KEY_get0_public_key(src));

		pkcs15_key->algorithm = SC_ALGORITHM_EC;

		if (!sc_pkcs15_convert_bignum(&dst->privateD, EC_KEY_get0_private_key(src)))
			return SC_ERROR_INCOMPATIBLE_KEY;

		grp = EC_KEY_get0_group(src);
		if(grp == 0)
			return SC_ERROR_INCOMPATIBLE_KEY;

		/* get curve name */
		nid = EC_GROUP_get_curve_name(grp);
		if(nid != 0) {
			const char *sn = OBJ_nid2sn(nid);
			if (sn)
				dst->params.named_curve = strdup(sn);
		}

		/* Decode EC_POINT from a octet string */
		buflen = EC_POINT_point2oct(grp, (const EC_POINT *) EC_KEY_get0_public_key(src),
				POINT_CONVERSION_UNCOMPRESSED, buf, buflen, NULL);
		if (!buflen)
			return SC_ERROR_INCOMPATIBLE_KEY;

		/* copy the public key */
		dst->ecpointQ.value = malloc(buflen);
		if (!dst->ecpointQ.value)
			return SC_ERROR_OUT_OF_MEMORY;
		memcpy(dst->ecpointQ.value, buf, buflen);
		dst->ecpointQ.len = buflen;

		/*
		 * In OpenSC the field_length is in bits. Not all curves are a mutiple of 8.
		 * EC_POINT_point2oct handles this and returns octstrings that can handle
		 * these curves. Get real field_length from OpenSSL. 
		 */
		dst->params.field_length = EC_GROUP_get_degree(grp);

		/* Octetstring may need leading zeros if BN is to short */
		if (dst->privateD.len < (dst->params.field_length + 7) / 8)   {
			size_t d = (dst->params.field_length + 7) / 8 - dst->privateD.len;

			dst->privateD.data = realloc(dst->privateD.data, dst->privateD.len + d);
			if (!dst->privateD.data)
				return SC_ERROR_OUT_OF_MEMORY;

			memmove(dst->privateD.data + d, dst->privateD.data, dst->privateD.len);
			memset(dst->privateD.data, 0, d);

			dst->privateD.len += d;
		}

		break;
	}
#endif /* OPENSSL_VERSION_NUMBER >= 0x10000000L && !defined(OPENSSL_NO_EC) */
	default:
		return SC_ERROR_NOT_SUPPORTED;
	}

	return SC_SUCCESS;
#else
	return SC_ERROR_NOT_IMPLEMENTED;
#endif
}