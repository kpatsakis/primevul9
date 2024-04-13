static zval *to_zval_array(encodeTypePtr type, xmlNodePtr data TSRMLS_DC)
{
	zval *ret;
	xmlNodePtr trav;
	encodePtr enc = NULL;
	int dimension = 1;
	int* dims = NULL;
	int* pos = NULL;
	xmlAttrPtr attr;
	sdlPtr sdl;
	sdlAttributePtr *arrayType;
	sdlExtraAttributePtr *ext;
	sdlTypePtr elementType;

	MAKE_STD_ZVAL(ret);
	FIND_XML_NULL(data, ret);
	sdl = SOAP_GLOBAL(sdl);

	if (data &&
	    (attr = get_attribute(data->properties,"arrayType")) &&
	    attr->children && attr->children->content) {
		char *type, *end, *ns;
		xmlNsPtr nsptr;

		parse_namespace(attr->children->content, &type, &ns);
		nsptr = xmlSearchNs(attr->doc, attr->parent, BAD_CAST(ns));

		end = strrchr(type,'[');
		if (end) {
			*end = '\0';
			dimension = calc_dimension(end+1);
			dims = get_position(dimension, end+1);
		}
		if (nsptr != NULL) {
			enc = get_encoder(SOAP_GLOBAL(sdl), (char*)nsptr->href, type);
		}
		efree(type);
		if (ns) {efree(ns);}

	} else if ((attr = get_attribute(data->properties,"itemType")) &&
	    attr->children &&
	    attr->children->content) {
		char *type, *ns;
		xmlNsPtr nsptr;

		parse_namespace(attr->children->content, &type, &ns);
		nsptr = xmlSearchNs(attr->doc, attr->parent, BAD_CAST(ns));
		if (nsptr != NULL) {
			enc = get_encoder(SOAP_GLOBAL(sdl), (char*)nsptr->href, type);
		}
		efree(type);
		if (ns) {efree(ns);}

		if ((attr = get_attribute(data->properties,"arraySize")) &&
		    attr->children && attr->children->content) {
			dimension = calc_dimension_12((char*)attr->children->content);
			dims = get_position_12(dimension, (char*)attr->children->content);
		} else {
			dims = emalloc(sizeof(int));
			*dims = 0;
		}

	} else if ((attr = get_attribute(data->properties,"arraySize")) &&
	    attr->children && attr->children->content) {

		dimension = calc_dimension_12((char*)attr->children->content);
		dims = get_position_12(dimension, (char*)attr->children->content);

	} else if (type->sdl_type != NULL &&
	           type->sdl_type->attributes != NULL &&
	           zend_hash_find(type->sdl_type->attributes, SOAP_1_1_ENC_NAMESPACE":arrayType",
	                          sizeof(SOAP_1_1_ENC_NAMESPACE":arrayType"),
	                          (void **)&arrayType) == SUCCESS &&
	           (*arrayType)->extraAttributes &&
	           zend_hash_find((*arrayType)->extraAttributes, WSDL_NAMESPACE":arrayType", sizeof(WSDL_NAMESPACE":arrayType"), (void **)&ext) == SUCCESS) {
		char *type, *end;

		type = estrdup((*ext)->val);
		end = strrchr(type,'[');
		if (end) {
			*end = '\0';
		}
		if ((*ext)->ns != NULL) {
			enc = get_encoder(SOAP_GLOBAL(sdl), (*ext)->ns, type);
		}
		efree(type);

		dims = emalloc(sizeof(int));
		*dims = 0;

	} else if (type->sdl_type != NULL &&
	           type->sdl_type->attributes != NULL &&
	           zend_hash_find(type->sdl_type->attributes, SOAP_1_2_ENC_NAMESPACE":itemType",
	                          sizeof(SOAP_1_2_ENC_NAMESPACE":itemType"),
	                          (void **)&arrayType) == SUCCESS &&
	           (*arrayType)->extraAttributes &&
	           zend_hash_find((*arrayType)->extraAttributes, WSDL_NAMESPACE":itemType", sizeof(WSDL_NAMESPACE":itemType"), (void **)&ext) == SUCCESS) {

		if ((*ext)->ns != NULL) {
			enc = get_encoder(SOAP_GLOBAL(sdl), (*ext)->ns, (*ext)->val);
		}

		if (zend_hash_find(type->sdl_type->attributes, SOAP_1_2_ENC_NAMESPACE":arraySize",
		                   sizeof(SOAP_1_2_ENC_NAMESPACE":arraySize"),
		                   (void **)&arrayType) == SUCCESS &&
		    (*arrayType)->extraAttributes &&
		    zend_hash_find((*arrayType)->extraAttributes, WSDL_NAMESPACE":arraySize", sizeof(WSDL_NAMESPACE":arraysize"), (void **)&ext) == SUCCESS) {
			dimension = calc_dimension_12((*ext)->val);
			dims = get_position_12(dimension, (*ext)->val);
		} else {
			dims = emalloc(sizeof(int));
			*dims = 0;
		}
	} else if (type->sdl_type != NULL &&
	           type->sdl_type->attributes != NULL &&
	           zend_hash_find(type->sdl_type->attributes, SOAP_1_2_ENC_NAMESPACE":arraySize",
	                          sizeof(SOAP_1_2_ENC_NAMESPACE":arraySize"),
	                          (void **)&arrayType) == SUCCESS &&
	           (*arrayType)->extraAttributes &&
	           zend_hash_find((*arrayType)->extraAttributes, WSDL_NAMESPACE":arraySize", sizeof(WSDL_NAMESPACE":arraysize"), (void **)&ext) == SUCCESS) {

		dimension = calc_dimension_12((*ext)->val);
		dims = get_position_12(dimension, (*ext)->val);
		if (type->sdl_type && type->sdl_type->elements &&
		    zend_hash_num_elements(type->sdl_type->elements) == 1 &&
		    (zend_hash_internal_pointer_reset(type->sdl_type->elements),
		     zend_hash_get_current_data(type->sdl_type->elements, (void**)&elementType) == SUCCESS) &&
		    (elementType = *(sdlTypePtr*)elementType) != NULL &&
		    elementType->encode) {
			enc = elementType->encode;
		}
	} else if (type->sdl_type && type->sdl_type->elements &&
	           zend_hash_num_elements(type->sdl_type->elements) == 1 &&
	           (zend_hash_internal_pointer_reset(type->sdl_type->elements),
	            zend_hash_get_current_data(type->sdl_type->elements, (void**)&elementType) == SUCCESS) &&
	           (elementType = *(sdlTypePtr*)elementType) != NULL &&
	           elementType->encode) {
		enc = elementType->encode;
	}
	if (dims == NULL) {
		dimension = 1;
		dims = emalloc(sizeof(int));
		*dims = 0;
	}
	pos = safe_emalloc(sizeof(int), dimension, 0);
	memset(pos,0,sizeof(int)*dimension);
	if (data &&
	    (attr = get_attribute(data->properties,"offset")) &&
	     attr->children && attr->children->content) {
		char* tmp = strrchr((char*)attr->children->content,'[');

		if (tmp == NULL) {
			tmp = (char*)attr->children->content;
		}
		get_position_ex(dimension, tmp, &pos);
	}

	array_init(ret);
	trav = data->children;
	while (trav) {
		if (trav->type == XML_ELEMENT_NODE) {
			int i;
			zval *tmpVal, *ar;
			xmlAttrPtr position = get_attribute(trav->properties,"position");

			tmpVal = master_to_zval(enc, trav TSRMLS_CC);
			if (position != NULL && position->children && position->children->content) {
				char* tmp = strrchr((char*)position->children->content, '[');
				if (tmp == NULL) {
					tmp = (char*)position->children->content;
				}
				get_position_ex(dimension, tmp, &pos);
			}

			/* Get/Create intermediate arrays for multidimensional arrays */
			i = 0;
			ar = ret;
			while (i < dimension-1) {
				zval** ar2;
				if (zend_hash_index_find(Z_ARRVAL_P(ar), pos[i], (void**)&ar2) == SUCCESS) {
					ar = *ar2;
				} else {
					zval *tmpAr;
					MAKE_STD_ZVAL(tmpAr);
					array_init(tmpAr);
					zend_hash_index_update(Z_ARRVAL_P(ar), pos[i], &tmpAr, sizeof(zval*), (void**)&ar2);
					ar = *ar2;
				}
				i++;
			}
			zend_hash_index_update(Z_ARRVAL_P(ar), pos[i], &tmpVal, sizeof(zval *), NULL);

			/* Increment position */
			i = dimension;
			while (i > 0) {
			  i--;
			  pos[i]++;
				if (pos[i] >= dims[i]) {
					if (i > 0) {
						pos[i] = 0;
					} else {
						/* TODO: Array index overflow */
					}
				} else {
				  break;
				}
			}
		}
		trav = trav->next;
	}
	efree(dims);
	efree(pos);
	return ret;
}