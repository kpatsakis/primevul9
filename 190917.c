   Create a MIME message based on given envelope and body sections */
PHP_FUNCTION(imap_mail_compose)
{
	zval *envelope, *body;
	zend_string *key;
	zval *data, *pvalue, *disp_data, *env_data;
	char *cookie = NIL;
	ENVELOPE *env;
	BODY *bod=NULL, *topbod=NULL;
	PART *mypart=NULL, *part;
	PARAMETER *param, *disp_param = NULL, *custom_headers_param = NULL, *tmp_param = NULL;
	char *tmp=NULL, *mystring=NULL, *t=NULL, *tempstring=NULL, *str_copy = NULL;
	int toppart = 0;
	int first;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "aa", &envelope, &body) == FAILURE) {
		return;
	}

#define PHP_RFC822_PARSE_ADRLIST(target, value) \
	str_copy = estrndup(Z_STRVAL_P(value), Z_STRLEN_P(value)); \
	rfc822_parse_adrlist(target, str_copy, "NO HOST"); \
	efree(str_copy);

	env = mail_newenvelope();
	if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(envelope), "remail", sizeof("remail") - 1)) != NULL) {
		convert_to_string_ex(pvalue);
		env->remail = cpystr(Z_STRVAL_P(pvalue));
	}
	if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(envelope), "return_path", sizeof("return_path") - 1)) != NULL) {
		convert_to_string_ex(pvalue);
		PHP_RFC822_PARSE_ADRLIST(&env->return_path, pvalue);
	}
	if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(envelope), "date", sizeof("date") - 1)) != NULL) {
		convert_to_string_ex(pvalue);
		env->date = (unsigned char*)cpystr(Z_STRVAL_P(pvalue));
	}
	if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(envelope), "from", sizeof("from") - 1)) != NULL) {
		convert_to_string_ex(pvalue);
		PHP_RFC822_PARSE_ADRLIST(&env->from, pvalue);
	}
	if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(envelope), "reply_to", sizeof("reply_to") - 1)) != NULL) {
		convert_to_string_ex(pvalue);
		PHP_RFC822_PARSE_ADRLIST(&env->reply_to, pvalue);
	}
	if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(envelope), "in_reply_to", sizeof("in_reply_to") - 1)) != NULL) {
		convert_to_string_ex(pvalue);
		env->in_reply_to = cpystr(Z_STRVAL_P(pvalue));
	}
	if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(envelope), "subject", sizeof("subject") - 1)) != NULL) {
		convert_to_string_ex(pvalue);
		env->subject = cpystr(Z_STRVAL_P(pvalue));
	}
	if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(envelope), "to", sizeof("to") - 1)) != NULL) {
		convert_to_string_ex(pvalue);
		PHP_RFC822_PARSE_ADRLIST(&env->to, pvalue);
	}
	if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(envelope), "cc", sizeof("cc") - 1)) != NULL) {
		convert_to_string_ex(pvalue);
		PHP_RFC822_PARSE_ADRLIST(&env->cc, pvalue);
	}
	if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(envelope), "bcc", sizeof("bcc") - 1)) != NULL) {
		convert_to_string_ex(pvalue);
		PHP_RFC822_PARSE_ADRLIST(&env->bcc, pvalue);
	}
	if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(envelope), "message_id", sizeof("message_id") - 1)) != NULL) {
		convert_to_string_ex(pvalue);
		env->message_id=cpystr(Z_STRVAL_P(pvalue));
	}

	if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(envelope), "custom_headers", sizeof("custom_headers") - 1)) != NULL) {
		if (Z_TYPE_P(pvalue) == IS_ARRAY) {
			custom_headers_param = tmp_param = NULL;
			ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(pvalue), env_data) {
				custom_headers_param = mail_newbody_parameter();
				convert_to_string_ex(env_data);
				custom_headers_param->value = (char *) fs_get(Z_STRLEN_P(env_data) + 1);
				custom_headers_param->attribute = NULL;
				memcpy(custom_headers_param->value, Z_STRVAL_P(env_data), Z_STRLEN_P(env_data) + 1);
				custom_headers_param->next = tmp_param;
				tmp_param = custom_headers_param;
			} ZEND_HASH_FOREACH_END();
		}
	}

	first = 1;
	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(body), data) {
		if (first) {
			first = 0;

			if (Z_TYPE_P(data) != IS_ARRAY) {
				php_error_docref(NULL, E_WARNING, "body parameter must be a non-empty array");
				RETURN_FALSE;
			}

			bod = mail_newbody();
			topbod = bod;

			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "type", sizeof("type") - 1)) != NULL) {
				convert_to_long_ex(pvalue);
				bod->type = (short) Z_LVAL_P(pvalue);
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "encoding", sizeof("encoding") - 1)) != NULL) {
				convert_to_long_ex(pvalue);
				bod->encoding = (short) Z_LVAL_P(pvalue);
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "charset", sizeof("charset") - 1)) != NULL) {
				convert_to_string_ex(pvalue);
				tmp_param = mail_newbody_parameter();
				tmp_param->value = cpystr(Z_STRVAL_P(pvalue));
				tmp_param->attribute = cpystr("CHARSET");
				tmp_param->next = bod->parameter;
				bod->parameter = tmp_param;
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "type.parameters", sizeof("type.parameters") - 1)) != NULL) {
				if(Z_TYPE_P(pvalue) == IS_ARRAY) {
					disp_param = tmp_param = NULL;
					ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(pvalue), key, disp_data) {
						disp_param = mail_newbody_parameter();
						disp_param->attribute = cpystr(ZSTR_VAL(key));
						convert_to_string_ex(disp_data);
						disp_param->value = (char *) fs_get(Z_STRLEN_P(disp_data) + 1);
						memcpy(disp_param->value, Z_STRVAL_P(disp_data), Z_STRLEN_P(disp_data) + 1);
						disp_param->next = tmp_param;
						tmp_param = disp_param;
					} ZEND_HASH_FOREACH_END();
					bod->parameter = disp_param;
				}
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "subtype", sizeof("subtype") - 1)) != NULL) {
				convert_to_string_ex(pvalue);
				bod->subtype = cpystr(Z_STRVAL_P(pvalue));
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "id", sizeof("id") - 1)) != NULL) {
				convert_to_string_ex(pvalue);
				bod->id = cpystr(Z_STRVAL_P(pvalue));
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "description", sizeof("description") - 1)) != NULL) {
				convert_to_string_ex(pvalue);
				bod->description = cpystr(Z_STRVAL_P(pvalue));
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "disposition.type", sizeof("disposition.type") - 1)) != NULL) {
				convert_to_string_ex(pvalue);
				bod->disposition.type = (char *) fs_get(Z_STRLEN_P(pvalue) + 1);
				memcpy(bod->disposition.type, Z_STRVAL_P(pvalue), Z_STRLEN_P(pvalue)+1);
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "disposition", sizeof("disposition") - 1)) != NULL) {
				if (Z_TYPE_P(pvalue) == IS_ARRAY) {
					disp_param = tmp_param = NULL;
					ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(pvalue), key, disp_data) {
						disp_param = mail_newbody_parameter();
						disp_param->attribute = cpystr(ZSTR_VAL(key));
						convert_to_string_ex(disp_data);
						disp_param->value = (char *) fs_get(Z_STRLEN_P(disp_data) + 1);
						memcpy(disp_param->value, Z_STRVAL_P(disp_data), Z_STRLEN_P(disp_data) + 1);
						disp_param->next = tmp_param;
						tmp_param = disp_param;
					} ZEND_HASH_FOREACH_END();
					bod->disposition.parameter = disp_param;
				}
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "contents.data", sizeof("contents.data") - 1)) != NULL) {
				convert_to_string_ex(pvalue);
				bod->contents.text.data = fs_get(Z_STRLEN_P(pvalue) + 1);
				memcpy(bod->contents.text.data, Z_STRVAL_P(pvalue), Z_STRLEN_P(pvalue)+1);
				bod->contents.text.size = Z_STRLEN_P(pvalue);
			} else {
				bod->contents.text.data = fs_get(1);
				memcpy(bod->contents.text.data, "", 1);
				bod->contents.text.size = 0;
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "lines", sizeof("lines") - 1)) != NULL) {
				convert_to_long_ex(pvalue);
				bod->size.lines = Z_LVAL_P(pvalue);
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "bytes", sizeof("bytes") - 1)) != NULL) {
				convert_to_long_ex(pvalue);
				bod->size.bytes = Z_LVAL_P(pvalue);
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "md5", sizeof("md5") - 1)) != NULL) {
				convert_to_string_ex(pvalue);
				bod->md5 = cpystr(Z_STRVAL_P(pvalue));
			}
		} else if (Z_TYPE_P(data) == IS_ARRAY) {
			short type = -1;
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "type", sizeof("type") - 1)) != NULL) {
				convert_to_long_ex(pvalue);
				type = (short) Z_LVAL_P(pvalue);
			}

			if (!toppart) {
				bod->nested.part = mail_newbody_part();
				mypart = bod->nested.part;
				toppart = 1;
			} else {
				mypart->next = mail_newbody_part();
				mypart = mypart->next;
			}

			bod = &mypart->body;

			if (type != TYPEMULTIPART) {
				bod->type = type;
			}

			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "encoding", sizeof("encoding") - 1)) != NULL) {
				convert_to_long_ex(pvalue);
				bod->encoding = (short) Z_LVAL_P(pvalue);
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "charset", sizeof("charset") - 1)) != NULL) {
				convert_to_string_ex(pvalue);
				tmp_param = mail_newbody_parameter();
				tmp_param->value = (char *) fs_get(Z_STRLEN_P(pvalue) + 1);
				memcpy(tmp_param->value, Z_STRVAL_P(pvalue), Z_STRLEN_P(pvalue) + 1);
				tmp_param->attribute = cpystr("CHARSET");
				tmp_param->next = bod->parameter;
				bod->parameter = tmp_param;
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "type.parameters", sizeof("type.parameters") - 1)) != NULL) {
				if (Z_TYPE_P(pvalue) == IS_ARRAY) {
					disp_param = tmp_param = NULL;
					ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(pvalue), key, disp_data) {
						disp_param = mail_newbody_parameter();
						disp_param->attribute = cpystr(ZSTR_VAL(key));
						convert_to_string_ex(disp_data);
						disp_param->value = (char *)fs_get(Z_STRLEN_P(disp_data) + 1);
						memcpy(disp_param->value, Z_STRVAL_P(disp_data), Z_STRLEN_P(disp_data) + 1);
						disp_param->next = tmp_param;
						tmp_param = disp_param;
					} ZEND_HASH_FOREACH_END();
					bod->parameter = disp_param;
				}
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "subtype", sizeof("subtype") - 1)) != NULL) {
				convert_to_string_ex(pvalue);
				bod->subtype = cpystr(Z_STRVAL_P(pvalue));
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "id", sizeof("id") - 1)) != NULL) {
				convert_to_string_ex(pvalue);
				bod->id = cpystr(Z_STRVAL_P(pvalue));
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "description", sizeof("description") - 1)) != NULL) {
				convert_to_string_ex(pvalue);
				bod->description = cpystr(Z_STRVAL_P(pvalue));
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "disposition.type", sizeof("disposition.type") - 1)) != NULL) {
				convert_to_string_ex(pvalue);
				bod->disposition.type = (char *) fs_get(Z_STRLEN_P(pvalue) + 1);
				memcpy(bod->disposition.type, Z_STRVAL_P(pvalue), Z_STRLEN_P(pvalue)+1);
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "disposition", sizeof("disposition") - 1)) != NULL) {
				if (Z_TYPE_P(pvalue) == IS_ARRAY) {
					disp_param = tmp_param = NULL;
					ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(pvalue), key, disp_data) {
						disp_param = mail_newbody_parameter();
						disp_param->attribute = cpystr(ZSTR_VAL(key));
						convert_to_string_ex(disp_data);
						disp_param->value = (char *) fs_get(Z_STRLEN_P(disp_data) + 1);
						memcpy(disp_param->value, Z_STRVAL_P(disp_data), Z_STRLEN_P(disp_data) + 1);
						disp_param->next = tmp_param;
						tmp_param = disp_param;
					} ZEND_HASH_FOREACH_END();
					bod->disposition.parameter = disp_param;
				}
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "contents.data", sizeof("contents.data") - 1)) != NULL) {
				convert_to_string_ex(pvalue);
				bod->contents.text.data = fs_get(Z_STRLEN_P(pvalue) + 1);
				memcpy(bod->contents.text.data, Z_STRVAL_P(pvalue), Z_STRLEN_P(pvalue) + 1);
				bod->contents.text.size = Z_STRLEN_P(pvalue);
			} else {
				bod->contents.text.data = fs_get(1);
				memcpy(bod->contents.text.data, "", 1);
				bod->contents.text.size = 0;
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "lines", sizeof("lines") - 1)) != NULL) {
				convert_to_long_ex(pvalue);
				bod->size.lines = Z_LVAL_P(pvalue);
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "bytes", sizeof("bytes") - 1)) != NULL) {
				convert_to_long_ex(pvalue);
				bod->size.bytes = Z_LVAL_P(pvalue);
			}
			if ((pvalue = zend_hash_str_find(Z_ARRVAL_P(data), "md5", sizeof("md5") - 1)) != NULL) {
				convert_to_string_ex(pvalue);
				bod->md5 = cpystr(Z_STRVAL_P(pvalue));
			}
		}
	} ZEND_HASH_FOREACH_END();

	if (first) {
		php_error_docref(NULL, E_WARNING, "body parameter must be a non-empty array");
		RETURN_FALSE;
	}

	if (bod && bod->type == TYPEMULTIPART && (!bod->nested.part || !bod->nested.part->next)) {
		php_error_docref(NULL, E_WARNING, "cannot generate multipart e-mail without components.");
		RETVAL_FALSE;
		goto done;
	}

	rfc822_encode_body_7bit(env, topbod);

	tmp = emalloc(SENDBUFLEN + 1);

	rfc822_header(tmp, env, topbod);

	/* add custom envelope headers */
	if (custom_headers_param) {
		int l = strlen(tmp) - 2, l2;
		PARAMETER *tp = custom_headers_param;

		/* remove last CRLF from tmp */
		tmp[l] = '\0';
		tempstring = emalloc(l);
		memcpy(tempstring, tmp, l);

		do {
			l2 = strlen(custom_headers_param->value);
			tempstring = erealloc(tempstring, l + l2 + CRLF_LEN + 1);
			memcpy(tempstring + l, custom_headers_param->value, l2);
			memcpy(tempstring + l + l2, CRLF, CRLF_LEN);
			l += l2 + CRLF_LEN;
		} while ((custom_headers_param = custom_headers_param->next));

		mail_free_body_parameter(&tp);

		mystring = emalloc(l + CRLF_LEN + 1);
		memcpy(mystring, tempstring, l);
		memcpy(mystring + l , CRLF, CRLF_LEN);
		mystring[l + CRLF_LEN] = '\0';

		efree(tempstring);
	} else {
		mystring = estrdup(tmp);
	}

	bod = topbod;

	if (bod && bod->type == TYPEMULTIPART) {

		/* first body part */
			part = bod->nested.part;

		/* find cookie */
			for (param = bod->parameter; param && !cookie; param = param->next) {
				if (!strcmp (param->attribute, "BOUNDARY")) {
					cookie = param->value;
				}
			}

		/* yucky default */
			if (!cookie) {
				cookie = "-";
			} else if (strlen(cookie) > (SENDBUFLEN - 2 - 2 - 2)) { /* validate cookie length -- + CRLF * 2 */
				php_error_docref(NULL, E_WARNING, "The boundary should be no longer than 4kb");
				RETVAL_FALSE;
				goto done;
			}

		/* for each part */
			do {
				t = tmp;

			/* append mini-header */
				*t = '\0';
				rfc822_write_body_header(&t, &part->body);

			/* output cookie, mini-header, and contents */
				spprintf(&tempstring, 0, "%s--%s%s%s%s", mystring, cookie, CRLF, tmp, CRLF);
				efree(mystring);
				mystring=tempstring;

				bod=&part->body;

				spprintf(&tempstring, 0, "%s%s%s", mystring, bod->contents.text.data, CRLF);
				efree(mystring);
				mystring=tempstring;
			} while ((part = part->next)); /* until done */

			/* output trailing cookie */
			spprintf(&tempstring, 0, "%s--%s--%s", mystring, cookie, CRLF);
			efree(mystring);
			mystring=tempstring;
	} else if (bod) {
		spprintf(&tempstring, 0, "%s%s%s", mystring, bod->contents.text.data, CRLF);
		efree(mystring);
		mystring=tempstring;
	} else {
		efree(mystring);
		RETVAL_FALSE;
		goto done;
	}

	RETVAL_STRING(tempstring);
	efree(tempstring);
done:
	if (tmp) {
		efree(tmp);
	}
	mail_free_body(&topbod);
	mail_free_envelope(&env);