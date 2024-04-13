ZEND_API int zend_declare_property_ex(zend_class_entry *ce, const char *name, int name_length, zval *property, int access_type, const char *doc_comment, int doc_comment_len TSRMLS_DC) /* {{{ */
{
	zend_property_info property_info, *property_info_ptr;
	const char *interned_name;
	ulong h = zend_get_hash_value(name, name_length+1);

	if (!(access_type & ZEND_ACC_PPP_MASK)) {
		access_type |= ZEND_ACC_PUBLIC;
	}
	if (access_type & ZEND_ACC_STATIC) {
		if (zend_hash_quick_find(&ce->properties_info, name, name_length + 1, h, (void**)&property_info_ptr) == SUCCESS &&
		    (property_info_ptr->flags & ZEND_ACC_STATIC) != 0) {
			property_info.offset = property_info_ptr->offset;
			zval_ptr_dtor(&ce->default_static_members_table[property_info.offset]);
			zend_hash_quick_del(&ce->properties_info, name, name_length + 1, h);
		} else {
			property_info.offset = ce->default_static_members_count++;
			ce->default_static_members_table = perealloc(ce->default_static_members_table, sizeof(zval*) * ce->default_static_members_count, ce->type == ZEND_INTERNAL_CLASS);
		}
		ce->default_static_members_table[property_info.offset] = property;
		if (ce->type == ZEND_USER_CLASS) {
			ce->static_members_table = ce->default_static_members_table;
		}
	} else {
		if (zend_hash_quick_find(&ce->properties_info, name, name_length + 1, h, (void**)&property_info_ptr) == SUCCESS &&
		    (property_info_ptr->flags & ZEND_ACC_STATIC) == 0) {
			property_info.offset = property_info_ptr->offset;
			zval_ptr_dtor(&ce->default_properties_table[property_info.offset]);
			zend_hash_quick_del(&ce->properties_info, name, name_length + 1, h);
		} else {
			property_info.offset = ce->default_properties_count++;
			ce->default_properties_table = perealloc(ce->default_properties_table, sizeof(zval*) * ce->default_properties_count, ce->type == ZEND_INTERNAL_CLASS);
		}
		ce->default_properties_table[property_info.offset] = property;
	}
	if (ce->type & ZEND_INTERNAL_CLASS) {
		switch(Z_TYPE_P(property)) {
			case IS_ARRAY:
			case IS_OBJECT:
			case IS_RESOURCE:
				zend_error(E_CORE_ERROR, "Internal zval's can't be arrays, objects or resources");
				break;
			default:
				break;
		}
	}
	switch (access_type & ZEND_ACC_PPP_MASK) {
		case ZEND_ACC_PRIVATE: {
				char *priv_name;
				int priv_name_length;

				zend_mangle_property_name(&priv_name, &priv_name_length, ce->name, ce->name_length, name, name_length, ce->type & ZEND_INTERNAL_CLASS);
				property_info.name = priv_name;
				property_info.name_length = priv_name_length;
			}
			break;
		case ZEND_ACC_PROTECTED: {
				char *prot_name;
				int prot_name_length;

				zend_mangle_property_name(&prot_name, &prot_name_length, "*", 1, name, name_length, ce->type & ZEND_INTERNAL_CLASS);
				property_info.name = prot_name;
				property_info.name_length = prot_name_length;
			}
			break;
		case ZEND_ACC_PUBLIC:
			if (IS_INTERNED(name)) {
				property_info.name = (char*)name;
			} else {
				property_info.name = ce->type & ZEND_INTERNAL_CLASS ? zend_strndup(name, name_length) : estrndup(name, name_length);
			}
			property_info.name_length = name_length;
			break;
	}

	interned_name = zend_new_interned_string(property_info.name, property_info.name_length+1, 0 TSRMLS_CC);
	if (interned_name != property_info.name) {
		if (ce->type == ZEND_USER_CLASS) {
			efree((char*)property_info.name);
		} else {
			free((char*)property_info.name);
		}
		property_info.name = interned_name;
	}

	property_info.flags = access_type;
	property_info.h = (access_type & ZEND_ACC_PUBLIC) ? h : zend_get_hash_value(property_info.name, property_info.name_length+1);

	property_info.doc_comment = doc_comment;
	property_info.doc_comment_len = doc_comment_len;

	property_info.ce = ce;

	zend_hash_quick_update(&ce->properties_info, name, name_length+1, h, &property_info, sizeof(zend_property_info), NULL);

	return SUCCESS;
}