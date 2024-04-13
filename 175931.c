static zval *to_zval_any(encodeTypePtr type, xmlNodePtr data TSRMLS_DC)
{
	xmlBufferPtr buf;
	zval *ret;

	if (SOAP_GLOBAL(sdl) && SOAP_GLOBAL(sdl)->elements && data->name) {
		smart_str nscat = {0};		
		sdlTypePtr *sdl_type;

		if (data->ns && data->ns->href) {
			smart_str_appends(&nscat, (char*)data->ns->href);
			smart_str_appendc(&nscat, ':');			
		}
		smart_str_appends(&nscat, (char*)data->name);
		smart_str_0(&nscat);

		if (zend_hash_find(SOAP_GLOBAL(sdl)->elements, nscat.c, nscat.len+1, (void **)&sdl_type) == SUCCESS &&
		    (*sdl_type)->encode) {
			smart_str_free(&nscat);
			return master_to_zval_int((*sdl_type)->encode, data TSRMLS_CC);
		}		
		smart_str_free(&nscat);
	}

	buf = xmlBufferCreate();
	xmlNodeDump(buf, NULL, data, 0, 0);
	MAKE_STD_ZVAL(ret);
	ZVAL_STRING(ret, (char*)xmlBufferContent(buf), 1);
	xmlBufferFree(buf);
	return ret;
}