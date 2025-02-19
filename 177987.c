int phar_verify_signature(php_stream *fp, size_t end_of_phar, php_uint32 sig_type, char *sig, int sig_len, char *fname, char **signature, int *signature_len, char **error) /* {{{ */
{
	int read_size, len;
	zend_off_t read_len;
	unsigned char buf[1024];

	php_stream_rewind(fp);

	switch (sig_type) {
		case PHAR_SIG_OPENSSL: {
#ifdef PHAR_HAVE_OPENSSL
			BIO *in;
			EVP_PKEY *key;
			EVP_MD *mdtype = (EVP_MD *) EVP_sha1();
			EVP_MD_CTX md_ctx;
#else
			int tempsig;
#endif
			zend_string *pubkey = NULL;
			char *pfile;
			php_stream *pfp;
#ifndef PHAR_HAVE_OPENSSL
			if (!zend_hash_str_exists(&module_registry, "openssl", sizeof("openssl")-1)) {
				if (error) {
					spprintf(error, 0, "openssl not loaded");
				}
				return FAILURE;
			}
#endif
			/* use __FILE__ . '.pubkey' for public key file */
			spprintf(&pfile, 0, "%s.pubkey", fname);
			pfp = php_stream_open_wrapper(pfile, "rb", 0, NULL);
			efree(pfile);

			if (!pfp || !(pubkey = php_stream_copy_to_mem(pfp, PHP_STREAM_COPY_ALL, 0)) || !ZSTR_LEN(pubkey)) {
				if (pfp) {
					php_stream_close(pfp);
				}
				if (error) {
					spprintf(error, 0, "openssl public key could not be read");
				}
				return FAILURE;
			}

			php_stream_close(pfp);
#ifndef PHAR_HAVE_OPENSSL
			tempsig = sig_len;

			if (FAILURE == phar_call_openssl_signverify(0, fp, end_of_phar, pubkey ? ZSTR_VAL(pubkey) : NULL, pubkey ? ZSTR_LEN(pubkey) : 0, &sig, &tempsig)) {
				if (pubkey) {
					zend_string_release(pubkey);
				}

				if (error) {
					spprintf(error, 0, "openssl signature could not be verified");
				}

				return FAILURE;
			}

			if (pubkey) {
				zend_string_release(pubkey);
			}

			sig_len = tempsig;
#else
			in = BIO_new_mem_buf(pubkey ? ZSTR_VAL(pubkey) : NULL, pubkey ? ZSTR_LEN(pubkey) : 0);

			if (NULL == in) {
				zend_string_release(pubkey);
				if (error) {
					spprintf(error, 0, "openssl signature could not be processed");
				}
				return FAILURE;
			}

			key = PEM_read_bio_PUBKEY(in, NULL,NULL, NULL);
			BIO_free(in);
			zend_string_release(pubkey);

			if (NULL == key) {
				if (error) {
					spprintf(error, 0, "openssl signature could not be processed");
				}
				return FAILURE;
			}

			EVP_VerifyInit(&md_ctx, mdtype);
			read_len = end_of_phar;

			if (read_len > sizeof(buf)) {
				read_size = sizeof(buf);
			} else {
				read_size = (int)read_len;
			}

			php_stream_seek(fp, 0, SEEK_SET);

			while (read_size && (len = php_stream_read(fp, (char*)buf, read_size)) > 0) {
				EVP_VerifyUpdate (&md_ctx, buf, len);
				read_len -= (zend_off_t)len;

				if (read_len < read_size) {
					read_size = (int)read_len;
				}
			}

			if (EVP_VerifyFinal(&md_ctx, (unsigned char *)sig, sig_len, key) != 1) {
				/* 1: signature verified, 0: signature does not match, -1: failed signature operation */
				EVP_MD_CTX_cleanup(&md_ctx);

				if (error) {
					spprintf(error, 0, "broken openssl signature");
				}

				return FAILURE;
			}

			EVP_MD_CTX_cleanup(&md_ctx);
#endif

			*signature_len = phar_hex_str((const char*)sig, sig_len, signature);
		}
		break;
#ifdef PHAR_HASH_OK
		case PHAR_SIG_SHA512: {
			unsigned char digest[64];
			PHP_SHA512_CTX context;

			if (sig_len < sizeof(digest)) {
				if (error) {
					spprintf(error, 0, "broken signature");
				}
				return FAILURE;
			}

			PHP_SHA512Init(&context);
			read_len = end_of_phar;

			if (read_len > sizeof(buf)) {
				read_size = sizeof(buf);
			} else {
				read_size = (int)read_len;
			}

			while ((len = php_stream_read(fp, (char*)buf, read_size)) > 0) {
				PHP_SHA512Update(&context, buf, len);
				read_len -= (zend_off_t)len;
				if (read_len < read_size) {
					read_size = (int)read_len;
				}
			}

			PHP_SHA512Final(digest, &context);

			if (memcmp(digest, sig, sizeof(digest))) {
				if (error) {
					spprintf(error, 0, "broken signature");
				}
				return FAILURE;
			}

			*signature_len = phar_hex_str((const char*)digest, sizeof(digest), signature);
			break;
		}
		case PHAR_SIG_SHA256: {
			unsigned char digest[32];
			PHP_SHA256_CTX context;

			if (sig_len < sizeof(digest)) {
				if (error) {
					spprintf(error, 0, "broken signature");
				}
				return FAILURE;
			}

			PHP_SHA256Init(&context);
			read_len = end_of_phar;

			if (read_len > sizeof(buf)) {
				read_size = sizeof(buf);
			} else {
				read_size = (int)read_len;
			}

			while ((len = php_stream_read(fp, (char*)buf, read_size)) > 0) {
				PHP_SHA256Update(&context, buf, len);
				read_len -= (zend_off_t)len;
				if (read_len < read_size) {
					read_size = (int)read_len;
				}
			}

			PHP_SHA256Final(digest, &context);

			if (memcmp(digest, sig, sizeof(digest))) {
				if (error) {
					spprintf(error, 0, "broken signature");
				}
				return FAILURE;
			}

			*signature_len = phar_hex_str((const char*)digest, sizeof(digest), signature);
			break;
		}
#else
		case PHAR_SIG_SHA512:
		case PHAR_SIG_SHA256:
			if (error) {
				spprintf(error, 0, "unsupported signature");
			}
			return FAILURE;
#endif
		case PHAR_SIG_SHA1: {
			unsigned char digest[20];
			PHP_SHA1_CTX  context;

			if (sig_len < sizeof(digest)) {
				if (error) {
					spprintf(error, 0, "broken signature");
				}
				return FAILURE;
			}

			PHP_SHA1Init(&context);
			read_len = end_of_phar;

			if (read_len > sizeof(buf)) {
				read_size = sizeof(buf);
			} else {
				read_size = (int)read_len;
			}

			while ((len = php_stream_read(fp, (char*)buf, read_size)) > 0) {
				PHP_SHA1Update(&context, buf, len);
				read_len -= (zend_off_t)len;
				if (read_len < read_size) {
					read_size = (int)read_len;
				}
			}

			PHP_SHA1Final(digest, &context);

			if (memcmp(digest, sig, sizeof(digest))) {
				if (error) {
					spprintf(error, 0, "broken signature");
				}
				return FAILURE;
			}

			*signature_len = phar_hex_str((const char*)digest, sizeof(digest), signature);
			break;
		}
		case PHAR_SIG_MD5: {
			unsigned char digest[16];
			PHP_MD5_CTX   context;

			if (sig_len < sizeof(digest)) {
				if (error) {
					spprintf(error, 0, "broken signature");
				}
				return FAILURE;
			}

			PHP_MD5Init(&context);
			read_len = end_of_phar;

			if (read_len > sizeof(buf)) {
				read_size = sizeof(buf);
			} else {
				read_size = (int)read_len;
			}

			while ((len = php_stream_read(fp, (char*)buf, read_size)) > 0) {
				PHP_MD5Update(&context, buf, len);
				read_len -= (zend_off_t)len;
				if (read_len < read_size) {
					read_size = (int)read_len;
				}
			}

			PHP_MD5Final(digest, &context);

			if (memcmp(digest, sig, sizeof(digest))) {
				if (error) {
					spprintf(error, 0, "broken signature");
				}
				return FAILURE;
			}

			*signature_len = phar_hex_str((const char*)digest, sizeof(digest), signature);
			break;
		}
		default:
			if (error) {
				spprintf(error, 0, "broken or unsupported signature");
			}
			return FAILURE;
	}
	return SUCCESS;
}