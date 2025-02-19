PHP_FUNCTION(xml_set_object)
{
	xml_parser *parser;
	zval *pind, *mythis;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ro", &pind, &mythis) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(parser,xml_parser *, &pind, -1, "XML Parser", le_xml_parser);

	/* please leave this commented - or ask thies@thieso.net before doing it (again) */
	if (parser->object) {
		zval_ptr_dtor(&parser->object);
	}

	/* please leave this commented - or ask thies@thieso.net before doing it (again) */
/* #ifdef ZEND_ENGINE_2
	zval_add_ref(&parser->object);
#endif */

	ALLOC_ZVAL(parser->object);
	MAKE_COPY_ZVAL(&mythis, parser->object);

	RETVAL_TRUE;
}