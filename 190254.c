PHP_FUNCTION(iconv_mime_decode_headers)
{
	const char *encoded_str;
	int encoded_str_len;
	char *charset = get_internal_encoding(TSRMLS_C);
	int charset_len = 0;
	long mode = 0;

	php_iconv_err_t err = PHP_ICONV_ERR_SUCCESS;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|ls",
		&encoded_str, &encoded_str_len, &mode, &charset, &charset_len) == FAILURE) {

		RETURN_FALSE;
	}

	if (charset_len >= ICONV_CSNMAXLEN) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Charset parameter exceeds the maximum allowed length of %d characters", ICONV_CSNMAXLEN);
		RETURN_FALSE;
	}

	array_init(return_value);

	while (encoded_str_len > 0) {
		smart_str decoded_header = {0};
		char *header_name = NULL;
		size_t header_name_len = 0;
		char *header_value = NULL;
		size_t header_value_len = 0;
		char *p, *limit;
		const char *next_pos;

		if (PHP_ICONV_ERR_SUCCESS != (err = _php_iconv_mime_decode(&decoded_header, encoded_str, encoded_str_len, charset, &next_pos, mode))) {
			smart_str_free(&decoded_header);
			break;
		}

		if (decoded_header.c == NULL) {
			break;
		}

		limit = decoded_header.c + decoded_header.len;
		for (p = decoded_header.c; p < limit; p++) {
			if (*p == ':') {
				*p = '\0';
				header_name = decoded_header.c;
				header_name_len = (p - decoded_header.c) + 1;

				while (++p < limit) {
					if (*p != ' ' && *p != '\t') {
						break;
					}
				}

				header_value = p;
				header_value_len = limit - p;

				break;
			}
		}

		if (header_name != NULL) {
			zval **elem, *new_elem;

			if (zend_hash_find(Z_ARRVAL_P(return_value), header_name, header_name_len, (void **)&elem) == SUCCESS) {
				if (Z_TYPE_PP(elem) != IS_ARRAY) {
					MAKE_STD_ZVAL(new_elem);
					array_init(new_elem);

					Z_ADDREF_PP(elem);
					add_next_index_zval(new_elem, *elem);

					zend_hash_update(Z_ARRVAL_P(return_value), header_name, header_name_len, (void *)&new_elem, sizeof(new_elem), NULL);

					elem = &new_elem;
				}
				add_next_index_stringl(*elem, header_value, header_value_len, 1);
			} else {
				add_assoc_stringl_ex(return_value, header_name, header_name_len, header_value, header_value_len, 1);
			}
		}
		encoded_str_len -= next_pos - encoded_str;
		encoded_str = next_pos;

		smart_str_free(&decoded_header);
	}

	if (err != PHP_ICONV_ERR_SUCCESS) {
		_php_iconv_show_error(err, charset, "???" TSRMLS_CC);
		zval_dtor(return_value);
		RETVAL_FALSE;
	}
}