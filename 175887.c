static xmlNodePtr to_xml_string(encodeTypePtr type, zval *data, int style, xmlNodePtr parent TSRMLS_DC)
{
	xmlNodePtr ret, text;
	char *str;
	int new_len;

	ret = xmlNewNode(NULL, BAD_CAST("BOGUS"));
	xmlAddChild(parent, ret);
	FIND_ZVAL_NULL(data, ret, style);

	if (Z_TYPE_P(data) == IS_STRING) {
		str = estrndup(Z_STRVAL_P(data), Z_STRLEN_P(data));
		new_len = Z_STRLEN_P(data);
	} else {
		zval tmp = *data;

		zval_copy_ctor(&tmp);
		convert_to_string(&tmp);
		str = estrndup(Z_STRVAL(tmp), Z_STRLEN(tmp));
		new_len = Z_STRLEN(tmp);
		zval_dtor(&tmp);
	}

	if (SOAP_GLOBAL(encoding) != NULL) {
		xmlBufferPtr in  = xmlBufferCreateStatic(str, new_len);
		xmlBufferPtr out = xmlBufferCreate();
		int n = xmlCharEncInFunc(SOAP_GLOBAL(encoding), out, in);

		if (n >= 0) {
			efree(str);
			str = estrdup((char*)xmlBufferContent(out));
			new_len = n;
		}
		xmlBufferFree(out);
		xmlBufferFree(in);
	}

	if (!php_libxml_xmlCheckUTF8(BAD_CAST(str))) {
		char *err = emalloc(new_len + 8);
		char c;
		int i;
		
		memcpy(err, str, new_len+1);
		for (i = 0; (c = err[i++]);) {
			if ((c & 0x80) == 0) {
			} else if ((c & 0xe0) == 0xc0) {
				if ((err[i] & 0xc0) != 0x80) {
					break;
				}
				i++;
			} else if ((c & 0xf0) == 0xe0) {
				if ((err[i] & 0xc0) != 0x80 || (err[i+1] & 0xc0) != 0x80) {
					break;
				}
				i += 2;
			} else if ((c & 0xf8) == 0xf0) {
				if ((err[i] & 0xc0) != 0x80 || (err[i+1] & 0xc0) != 0x80 || (err[i+2] & 0xc0) != 0x80) {
					break;
				}
				i += 3;
			} else {
				break;
			}
		}
		if (c) {
			err[i-1] = '\\';
			err[i++] = 'x';
			err[i++] = ((unsigned char)c >> 4) + ((((unsigned char)c >> 4) > 9) ? ('a' - 10) : '0');
			err[i++] = (c & 15) + (((c & 15) > 9) ? ('a' - 10) : '0');
			err[i++] = '.';
			err[i++] = '.';
			err[i++] = '.';
			err[i++] = 0;
		}

		soap_error1(E_ERROR,  "Encoding: string '%s' is not a valid utf-8 string", err);
	}

	text = xmlNewTextLen(BAD_CAST(str), new_len);
	xmlAddChild(ret, text);
	efree(str);

	if (style == SOAP_ENCODED) {
		set_ns_and_type(ret, type);
	}
	return ret;
}