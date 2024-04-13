ZEND_API void zend_update_class_constants(zend_class_entry *class_type TSRMLS_DC) /* {{{ */
{
	if ((class_type->ce_flags & ZEND_ACC_CONSTANTS_UPDATED) == 0 || (!CE_STATIC_MEMBERS(class_type) && class_type->default_static_members_count)) {
		zend_class_entry **scope = EG(in_execution)?&EG(scope):&CG(active_class_entry);
		zend_class_entry *old_scope = *scope;
		int i;

		*scope = class_type;
		zend_hash_apply_with_argument(&class_type->constants_table, (apply_func_arg_t) zval_update_constant, (void *)1 TSRMLS_CC);

		for (i = 0; i < class_type->default_properties_count; i++) {
			if (class_type->default_properties_table[i]) {
				zval_update_class_constant(&class_type->default_properties_table[i], 0, i TSRMLS_CC);
			}
		}

		if (!CE_STATIC_MEMBERS(class_type) && class_type->default_static_members_count) {
			zval **p;

			if (class_type->parent) {
				zend_update_class_constants(class_type->parent TSRMLS_CC);
			}
#if ZTS
			CG(static_members_table)[(zend_intptr_t)(class_type->static_members_table)] = emalloc(sizeof(zval*) * class_type->default_static_members_count);
#else
			class_type->static_members_table = emalloc(sizeof(zval*) * class_type->default_static_members_count);
#endif
			for (i = 0; i < class_type->default_static_members_count; i++) {
				p = &class_type->default_static_members_table[i];
				if (Z_ISREF_PP(p) &&
					class_type->parent &&
					i < class_type->parent->default_static_members_count &&
					*p == class_type->parent->default_static_members_table[i] &&
					CE_STATIC_MEMBERS(class_type->parent)[i]
				) {
					zval *q = CE_STATIC_MEMBERS(class_type->parent)[i];

					Z_ADDREF_P(q);
					Z_SET_ISREF_P(q);
					CE_STATIC_MEMBERS(class_type)[i] = q;
				} else {
					zval *r;

					ALLOC_ZVAL(r);
					*r = **p;
					INIT_PZVAL(r);
					zval_copy_ctor(r);
					CE_STATIC_MEMBERS(class_type)[i] = r;
				}
			}
		}

		for (i = 0; i < class_type->default_static_members_count; i++) {
			zval_update_class_constant(&CE_STATIC_MEMBERS(class_type)[i], 1, i TSRMLS_CC);
		}

		*scope = old_scope;
		class_type->ce_flags |= ZEND_ACC_CONSTANTS_UPDATED;
	}
}