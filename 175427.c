void _xml_startElementHandler(void *userData, const XML_Char *name, const XML_Char **attributes)
{
	xml_parser *parser = (xml_parser *)userData;
	const char **attrs = (const char **) attributes;
	char *tag_name;
	char *att, *val;
	int val_len;
	zval *retval, *args[3];

	if (parser) {
		parser->level++;

		tag_name = _xml_decode_tag(parser, name);

		if (parser->startElementHandler) {
			args[0] = _xml_resource_zval(parser->index);
			args[1] = _xml_string_zval(((char *) tag_name) + parser->toffset);
			MAKE_STD_ZVAL(args[2]);
			array_init(args[2]);

			while (attributes && *attributes) {
				att = _xml_decode_tag(parser, attributes[0]);
				val = xml_utf8_decode(attributes[1], strlen(attributes[1]), &val_len, parser->target_encoding);

				add_assoc_stringl(args[2], att, val, val_len, 0);

				attributes += 2;

				efree(att);
			}

			if ((retval = xml_call_handler(parser, parser->startElementHandler, parser->startElementPtr, 3, args))) {
				zval_ptr_dtor(&retval);
			}
		}

		if (parser->data) {
			if (parser->level <= XML_MAXLEVEL)  {
				zval *tag, *atr;
				int atcnt = 0;

				MAKE_STD_ZVAL(tag);
				MAKE_STD_ZVAL(atr);

				array_init(tag);
				array_init(atr);

				_xml_add_to_info(parser,((char *) tag_name) + parser->toffset);

				add_assoc_string(tag,"tag",((char *) tag_name) + parser->toffset,1); /* cast to avoid gcc-warning */
				add_assoc_string(tag,"type","open",1);
				add_assoc_long(tag,"level",parser->level);

				parser->ltags[parser->level-1] = estrdup(tag_name);
				parser->lastwasopen = 1;

				attributes = (const XML_Char **) attrs;

				while (attributes && *attributes) {
					att = _xml_decode_tag(parser, attributes[0]);
					val = xml_utf8_decode(attributes[1], strlen(attributes[1]), &val_len, parser->target_encoding);

					add_assoc_stringl(atr,att,val,val_len,0);

					atcnt++;
					attributes += 2;

					efree(att);
				}

				if (atcnt) {
					zend_hash_add(Z_ARRVAL_P(tag),"attributes",sizeof("attributes"),&atr,sizeof(zval*),NULL);
				} else {
					zval_ptr_dtor(&atr);
				}

				zend_hash_next_index_insert(Z_ARRVAL_P(parser->data),&tag,sizeof(zval*),(void *) &parser->ctag);
			} else if (parser->level == (XML_MAXLEVEL + 1)) {
				TSRMLS_FETCH();
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Maximum depth exceeded - Results truncated");
			}
		}

		efree(tag_name);
	}
}