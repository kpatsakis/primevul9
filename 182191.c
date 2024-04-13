sc_pkcs15_prkey_attrs_from_cert(struct sc_pkcs15_card *p15card, struct sc_pkcs15_object *cert_object,
		struct sc_pkcs15_object **out_key_object)
{
	struct sc_context *ctx = p15card->card->ctx;
#ifdef ENABLE_OPENSSL
	struct sc_pkcs15_object *key_object = NULL;
	struct sc_pkcs15_prkey_info *key_info = NULL;
	X509 *x = NULL;
	BIO *mem = NULL;
	unsigned char *buff = NULL, *ptr = NULL;
	int rv;

	LOG_FUNC_CALLED(ctx);
	if (out_key_object)
		*out_key_object = NULL;

	rv = sc_pkcs15_find_prkey_by_id(p15card, &((struct sc_pkcs15_cert_info *)cert_object->data)->id, &key_object);
	if (rv == SC_ERROR_OBJECT_NOT_FOUND)
		LOG_FUNC_RETURN(ctx, SC_SUCCESS);
	LOG_TEST_RET(ctx, rv, "Find private key error");

	key_info = (struct sc_pkcs15_prkey_info *) key_object->data;

	ERR_load_ERR_strings();
	ERR_load_crypto_strings();

	sc_log(ctx, "CertValue(%"SC_FORMAT_LEN_SIZE_T"u) %p",
	       cert_object->content.len, cert_object->content.value);
	mem = BIO_new_mem_buf(cert_object->content.value, cert_object->content.len);
	if (!mem)
		LOG_TEST_RET(ctx, SC_ERROR_INTERNAL, "MEM buffer allocation error");

	x = d2i_X509_bio(mem, NULL);
	if (!x)
		LOG_TEST_RET(ctx, SC_ERROR_INTERNAL, "x509 parse error");

	buff = OPENSSL_malloc(i2d_X509(x,NULL) + EVP_MAX_MD_SIZE);
	if (!buff)
		LOG_TEST_RET(ctx, SC_ERROR_OUT_OF_MEMORY, "OpenSSL allocation error");

	ptr = buff;
	rv = i2d_X509_NAME(X509_get_subject_name(x), &ptr);
	if (rv <= 0)
		LOG_TEST_RET(ctx, SC_ERROR_INTERNAL, "Get subject name error");

	key_info->subject.value = malloc(rv);
	if (!key_info->subject.value)
		LOG_TEST_RET(ctx, SC_ERROR_OUT_OF_MEMORY, "Subject allocation error");

	memcpy(key_info->subject.value, buff, rv);
	key_info->subject.len = rv;

	strlcpy(key_object->label, cert_object->label, sizeof(key_object->label));

	rv = 0;

	if (x)
		X509_free(x);
	if (mem)
		BIO_free(mem);
	if (buff)
		OPENSSL_free(buff);

	ERR_clear_error();
	ERR_free_strings();

	if (out_key_object)
		*out_key_object = key_object;

	sc_log(ctx, "Subject %s", sc_dump_hex(key_info->subject.value, key_info->subject.len));
	LOG_FUNC_RETURN(ctx, rv);
#else
	LOG_FUNC_RETURN(ctx, SC_ERROR_NOT_SUPPORTED);
#endif
}