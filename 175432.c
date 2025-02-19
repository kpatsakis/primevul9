void _xml_characterDataHandler(void *userData, const XML_Char *s, int len)
{
	xml_parser *parser = (xml_parser *)userData;

	if (parser) {
		zval *retval, *args[2];

		if (parser->characterDataHandler) {
			args[0] = _xml_resource_zval(parser->index);
			args[1] = _xml_xmlchar_zval(s, len, parser->target_encoding);
			if ((retval = xml_call_handler(parser, parser->characterDataHandler, parser->characterDataPtr, 2, args))) {
				zval_ptr_dtor(&retval);
			}
		}

		if (parser->data) {
			int i;
			int doprint = 0;

			char *decoded_value;
			int decoded_len;

			decoded_value = xml_utf8_decode(s,len,&decoded_len,parser->target_encoding);
			for (i = 0; i < decoded_len; i++) {
				switch (decoded_value[i]) {
				case ' ':
				case '\t':
				case '\n':
					continue;
				default:
					doprint = 1;
					break;
				}
				if (doprint) {
					break;
				}
			}
			if (doprint || (! parser->skipwhite)) {
				if (parser->lastwasopen) {
					zval **myval;

					/* check if the current tag already has a value - if yes append to that! */
					if (zend_hash_find(Z_ARRVAL_PP(parser->ctag),"value",sizeof("value"),(void **) &myval) == SUCCESS) {
						int newlen = Z_STRLEN_PP(myval) + decoded_len;
						Z_STRVAL_PP(myval) = erealloc(Z_STRVAL_PP(myval),newlen+1);
						strncpy(Z_STRVAL_PP(myval) + Z_STRLEN_PP(myval), decoded_value, decoded_len + 1);
						Z_STRLEN_PP(myval) += decoded_len;
						efree(decoded_value);
					} else {
						add_assoc_string(*(parser->ctag),"value",decoded_value,0);
					}

				} else {
					zval *tag;
					zval **curtag, **mytype, **myval;
					HashPosition hpos=NULL;

					zend_hash_internal_pointer_end_ex(Z_ARRVAL_P(parser->data), &hpos);

					if (hpos && (zend_hash_get_current_data_ex(Z_ARRVAL_P(parser->data), (void **) &curtag, &hpos) == SUCCESS)) {
						if (zend_hash_find(Z_ARRVAL_PP(curtag),"type",sizeof("type"),(void **) &mytype) == SUCCESS) {
							if (!strcmp(Z_STRVAL_PP(mytype), "cdata")) {
								if (zend_hash_find(Z_ARRVAL_PP(curtag),"value",sizeof("value"),(void **) &myval) == SUCCESS) {
									int newlen = Z_STRLEN_PP(myval) + decoded_len;
									Z_STRVAL_PP(myval) = erealloc(Z_STRVAL_PP(myval),newlen+1);
									strncpy(Z_STRVAL_PP(myval) + Z_STRLEN_PP(myval), decoded_value, decoded_len + 1);
									Z_STRLEN_PP(myval) += decoded_len;
									efree(decoded_value);
									return;
								}
							}
						}
					}

					if (parser->level <= XML_MAXLEVEL && parser->level > 0) {
						MAKE_STD_ZVAL(tag);

						array_init(tag);

						_xml_add_to_info(parser,parser->ltags[parser->level-1] + parser->toffset);

						add_assoc_string(tag,"tag",parser->ltags[parser->level-1] + parser->toffset,1);
						add_assoc_string(tag,"value",decoded_value,0);
						add_assoc_string(tag,"type","cdata",1);
						add_assoc_long(tag,"level",parser->level);

						zend_hash_next_index_insert(Z_ARRVAL_P(parser->data),&tag,sizeof(zval*),NULL);
					} else if (parser->level == (XML_MAXLEVEL + 1)) {
						TSRMLS_FETCH();
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "Maximum depth exceeded - Results truncated");
					}
				}
			} else {
				efree(decoded_value);
			}
		}
	}
}