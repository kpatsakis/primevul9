static void model_to_zval_object(zval *ret, sdlContentModelPtr model, xmlNodePtr data, sdlPtr sdl TSRMLS_DC)
{
	switch (model->kind) {
		case XSD_CONTENT_ELEMENT:
			if (model->u.element->name) {
				xmlNodePtr node = get_node(data->children, model->u.element->name);

				if (node) {
					zval *val;
					xmlNodePtr r_node;

					r_node = check_and_resolve_href(node);
					if (r_node && r_node->children && r_node->children->content) {
						if (model->u.element->fixed && strcmp(model->u.element->fixed, (char*)r_node->children->content) != 0) {
							soap_error3(E_ERROR, "Encoding: Element '%s' has fixed value '%s' (value '%s' is not allowed)", model->u.element->name, model->u.element->fixed, r_node->children->content);
						}
						val = master_to_zval(model->u.element->encode, r_node TSRMLS_CC);
					} else if (model->u.element->fixed) {
						xmlNodePtr dummy = xmlNewNode(NULL, BAD_CAST("BOGUS"));
						xmlNodeSetContent(dummy, BAD_CAST(model->u.element->fixed));
						val = master_to_zval(model->u.element->encode, dummy TSRMLS_CC);
						xmlFreeNode(dummy);
					} else if (model->u.element->def && !model->u.element->nillable) {
						xmlNodePtr dummy = xmlNewNode(NULL, BAD_CAST("BOGUS"));
						xmlNodeSetContent(dummy, BAD_CAST(model->u.element->def));
						val = master_to_zval(model->u.element->encode, dummy TSRMLS_CC);
						xmlFreeNode(dummy);
					} else {
						val = master_to_zval(model->u.element->encode, r_node TSRMLS_CC);
					}
					if ((node = get_node(node->next, model->u.element->name)) != NULL) {
						zval *array;

						MAKE_STD_ZVAL(array);
						array_init(array);
						add_next_index_zval(array, val);
						do {
							if (node && node->children && node->children->content) {
								if (model->u.element->fixed && strcmp(model->u.element->fixed, (char*)node->children->content) != 0) {
									soap_error3(E_ERROR, "Encoding: Element '%s' has fixed value '%s' (value '%s' is not allowed)", model->u.element->name, model->u.element->fixed, node->children->content);
								}
								val = master_to_zval(model->u.element->encode, node TSRMLS_CC);
							} else if (model->u.element->fixed) {
								xmlNodePtr dummy = xmlNewNode(NULL, BAD_CAST("BOGUS"));
								xmlNodeSetContent(dummy, BAD_CAST(model->u.element->fixed));
								val = master_to_zval(model->u.element->encode, dummy TSRMLS_CC);
								xmlFreeNode(dummy);
							} else if (model->u.element->def && !model->u.element->nillable) {
								xmlNodePtr dummy = xmlNewNode(NULL, BAD_CAST("BOGUS"));
								xmlNodeSetContent(dummy, BAD_CAST(model->u.element->def));
								val = master_to_zval(model->u.element->encode, dummy TSRMLS_CC);
								xmlFreeNode(dummy);
							} else {
								val = master_to_zval(model->u.element->encode, node TSRMLS_CC);
							}
							add_next_index_zval(array, val);
						} while ((node = get_node(node->next, model->u.element->name)) != NULL);
						val = array;
					} else if ((Z_TYPE_P(val) != IS_NULL || !model->u.element->nillable) &&
					           (SOAP_GLOBAL(features) & SOAP_SINGLE_ELEMENT_ARRAYS) &&
					           (model->max_occurs == -1 || model->max_occurs > 1)) {
						zval *array;

						MAKE_STD_ZVAL(array);
						array_init(array);
						add_next_index_zval(array, val);
						val = array;
					}
					set_zval_property(ret, model->u.element->name, val TSRMLS_CC);
				}
			}
			break;
		case XSD_CONTENT_ALL:
		case XSD_CONTENT_SEQUENCE:
		case XSD_CONTENT_CHOICE: {
			sdlContentModelPtr *tmp;
			HashPosition pos;
			sdlContentModelPtr any = NULL;

			zend_hash_internal_pointer_reset_ex(model->u.content, &pos);
			while (zend_hash_get_current_data_ex(model->u.content, (void**)&tmp, &pos) == SUCCESS) {
				if ((*tmp)->kind == XSD_CONTENT_ANY) {
					any = *tmp;
				} else {
					model_to_zval_object(ret, *tmp, data, sdl TSRMLS_CC);
				}
				zend_hash_move_forward_ex(model->u.content, &pos);
			}
			if (any) {
				model_to_zval_any(ret, data->children TSRMLS_CC);
			}
			break;
		}
		case XSD_CONTENT_GROUP:
			model_to_zval_object(ret, model->u.group->model, data, sdl TSRMLS_CC);
			break;
		default:
		  break;
	}
}