static xmlParserInputPtr _php_libxml_external_entity_loader(const char *URL,
		const char *ID, xmlParserCtxtPtr context)
{
	xmlParserInputPtr	ret			= NULL;
	const char			*resource	= NULL;
	zval				*public		= NULL,
						*system		= NULL,
						*ctxzv		= NULL,
						**params[]	= {&public, &system, &ctxzv},
						*retval_ptr	= NULL;
	int					retval;
	zend_fcall_info		*fci;
	TSRMLS_FETCH();

	fci = &LIBXML(entity_loader).fci;
	
	if (fci->size == 0) {
		/* no custom user-land callback set up; delegate to original loader */
		return _php_libxml_default_entity_loader(URL, ID, context);
	}

	ALLOC_INIT_ZVAL(public);
	if (ID != NULL) {
		ZVAL_STRING(public, ID, 1);
	}
	ALLOC_INIT_ZVAL(system);
	if (URL != NULL) {
		ZVAL_STRING(system, URL, 1);
	}
	MAKE_STD_ZVAL(ctxzv);
	array_init_size(ctxzv, 4);

#define ADD_NULL_OR_STRING_KEY(memb) \
	if (context->memb == NULL) { \
		add_assoc_null_ex(ctxzv, #memb, sizeof(#memb)); \
	} else { \
		add_assoc_string_ex(ctxzv, #memb, sizeof(#memb), \
				(char *)context->memb, 1); \
	}
	
	ADD_NULL_OR_STRING_KEY(directory)
	ADD_NULL_OR_STRING_KEY(intSubName)
	ADD_NULL_OR_STRING_KEY(extSubURI)
	ADD_NULL_OR_STRING_KEY(extSubSystem)
	
#undef ADD_NULL_OR_STRING_KEY
	
	fci->retval_ptr_ptr	= &retval_ptr;
	fci->params			= params;
	fci->param_count	= sizeof(params)/sizeof(*params);
	fci->no_separation	= 1;
	
	retval = zend_call_function(fci, &LIBXML(entity_loader).fcc TSRMLS_CC);
	if (retval != SUCCESS || fci->retval_ptr_ptr == NULL) {
		php_libxml_ctx_error(context,
				"Call to user entity loader callback '%s' has failed",
				fci->function_name);
	} else {
		retval_ptr = *fci->retval_ptr_ptr;
		if (retval_ptr == NULL) {
			php_libxml_ctx_error(context,
					"Call to user entity loader callback '%s' has failed; "
					"probably it has thrown an exception",
					fci->function_name);
		} else if (Z_TYPE_P(retval_ptr) == IS_STRING) {
is_string:
			resource = Z_STRVAL_P(retval_ptr);
		} else if (Z_TYPE_P(retval_ptr) == IS_RESOURCE) {
			php_stream *stream;
			php_stream_from_zval_no_verify(stream, &retval_ptr);
			if (stream == NULL) {
				php_libxml_ctx_error(context,
						"The user entity loader callback '%s' has returned a "
						"resource, but it is not a stream",
						fci->function_name);
			} else {
				/* TODO: allow storing the encoding in the stream context? */
				xmlCharEncoding enc = XML_CHAR_ENCODING_NONE;
				xmlParserInputBufferPtr pib = xmlAllocParserInputBuffer(enc);
				if (pib == NULL) {
					php_libxml_ctx_error(context, "Could not allocate parser "
							"input buffer");
				} else {
					/* make stream not being closed when the zval is freed */
					zend_list_addref(stream->rsrc_id);
					pib->context = stream;
					pib->readcallback = php_libxml_streams_IO_read;
					pib->closecallback = php_libxml_streams_IO_close;
					
					ret = xmlNewIOInputStream(context, pib, enc);
					if (ret == NULL) {
						xmlFreeParserInputBuffer(pib);
					}
				}
			}
		} else if (Z_TYPE_P(retval_ptr) != IS_NULL) {
			/* retval not string nor resource nor null; convert to string */
			SEPARATE_ZVAL(&retval_ptr);
			convert_to_string(retval_ptr);
			goto is_string;
		} /* else is null; don't try anything */
	}

	if (ret == NULL) {
		if (resource == NULL) {
			if (ID == NULL) {
				ID = "NULL";
			}
			php_libxml_ctx_error(context,
					"Failed to load external entity \"%s\"\n", ID);
		} else {
			/* we got the resource in the form of a string; open it */
			ret = xmlNewInputFromFile(context, resource);
		}
	}

	zval_ptr_dtor(&public);
	zval_ptr_dtor(&system);
	zval_ptr_dtor(&ctxzv);
	if (retval_ptr != NULL) {
		zval_ptr_dtor(&retval_ptr);
	}
	return ret;
}