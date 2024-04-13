PHP_FUNCTION(iconv_mime_encode)
{
	const char *field_name = NULL;
	int field_name_len;
	const char *field_value = NULL;
	int field_value_len;
	zval *pref = NULL;
	zval tmp_zv, *tmp_zv_p = NULL;
	smart_str retval = {0};
	php_iconv_err_t err;

	const char *in_charset = get_internal_encoding(TSRMLS_C);
	const char *out_charset = in_charset;
	long line_len = 76;
	const char *lfchars = "\r\n";
	php_iconv_enc_scheme_t scheme_id = PHP_ICONV_ENC_SCHEME_BASE64;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|a",
		&field_name, &field_name_len, &field_value, &field_value_len,
		&pref) == FAILURE) {

		RETURN_FALSE;
	}

	if (pref != NULL) {
		zval **ppval;

		if (zend_hash_find(Z_ARRVAL_P(pref), "scheme", sizeof("scheme"), (void **)&ppval) == SUCCESS) {
			if (Z_TYPE_PP(ppval) == IS_STRING && Z_STRLEN_PP(ppval) > 0) {
				switch (Z_STRVAL_PP(ppval)[0]) {
					case 'B': case 'b':
						scheme_id = PHP_ICONV_ENC_SCHEME_BASE64;
						break;

					case 'Q': case 'q':
						scheme_id = PHP_ICONV_ENC_SCHEME_QPRINT;
						break;
				}
			}
		}

		if (zend_hash_find(Z_ARRVAL_P(pref), "input-charset", sizeof("input-charset"), (void **)&ppval) == SUCCESS) {
			if (Z_STRLEN_PP(ppval) >= ICONV_CSNMAXLEN) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Charset parameter exceeds the maximum allowed length of %d characters", ICONV_CSNMAXLEN);
				RETURN_FALSE;
			}

			if (Z_TYPE_PP(ppval) == IS_STRING && Z_STRLEN_PP(ppval) > 0) {
				in_charset = Z_STRVAL_PP(ppval);
			}
		}


		if (zend_hash_find(Z_ARRVAL_P(pref), "output-charset", sizeof("output-charset"), (void **)&ppval) == SUCCESS) {
			if (Z_STRLEN_PP(ppval) >= ICONV_CSNMAXLEN) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Charset parameter exceeds the maximum allowed length of %d characters", ICONV_CSNMAXLEN);
				RETURN_FALSE;
			}

			if (Z_TYPE_PP(ppval) == IS_STRING && Z_STRLEN_PP(ppval) > 0) {
				out_charset = Z_STRVAL_PP(ppval);
			}
		}

		if (zend_hash_find(Z_ARRVAL_P(pref), "line-length", sizeof("line-length"), (void **)&ppval) == SUCCESS) {
			zval val, *pval = *ppval;

			if (Z_TYPE_P(pval) != IS_LONG) {
				val = *pval;
				zval_copy_ctor(&val);
				convert_to_long(&val);
				pval = &val;
			}

			line_len = Z_LVAL_P(pval);

			if (pval == &val) {
				zval_dtor(&val);
			}
		}

		if (zend_hash_find(Z_ARRVAL_P(pref), "line-break-chars", sizeof("line-break-chars"), (void **)&ppval) == SUCCESS) {
			if (Z_TYPE_PP(ppval) != IS_STRING) {
				tmp_zv = **ppval;
				zval_copy_ctor(&tmp_zv);
				convert_to_string(&tmp_zv);

				lfchars = Z_STRVAL(tmp_zv);

				tmp_zv_p = &tmp_zv;
			} else {
				lfchars = Z_STRVAL_PP(ppval);
			}
		}
	}

	err = _php_iconv_mime_encode(&retval, field_name, field_name_len,
		field_value, field_value_len, line_len, lfchars, scheme_id,
		out_charset, in_charset);
	_php_iconv_show_error(err, out_charset, in_charset TSRMLS_CC);

	if (err == PHP_ICONV_ERR_SUCCESS) {
		if (retval.c != NULL) {
			RETVAL_STRINGL(retval.c, retval.len, 0);
		} else {
			RETVAL_EMPTY_STRING();
		}
	} else {
		smart_str_free(&retval);
		RETVAL_FALSE;
	}

	if (tmp_zv_p != NULL) {
		zval_dtor(tmp_zv_p);
	}
}