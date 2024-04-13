static int zval_update_class_constant(zval **pp, int is_static, int offset TSRMLS_DC) /* {{{ */
{
	if (IS_CONSTANT_TYPE(Z_TYPE_PP(pp))) {
		zend_class_entry **scope = EG(in_execution)?&EG(scope):&CG(active_class_entry);

		if ((*scope)->parent) {
			zend_class_entry *ce = *scope;
			HashPosition pos;
			zend_property_info *prop_info;

			do {
				for (zend_hash_internal_pointer_reset_ex(&ce->properties_info, &pos);
				     zend_hash_get_current_data_ex(&ce->properties_info, (void **) &prop_info, &pos) == SUCCESS;
				     zend_hash_move_forward_ex(&ce->properties_info, &pos)) {
					if (is_static == ((prop_info->flags & ZEND_ACC_STATIC) != 0) &&
					    offset == prop_info->offset) {
						int ret;
						zend_class_entry *old_scope = *scope;
						*scope = prop_info->ce;
						ret = zval_update_constant(pp, 1 TSRMLS_CC);
						*scope = old_scope;
						return ret;
					}
				}
				ce = ce->parent;
			} while (ce);

		}
		return zval_update_constant(pp, 1 TSRMLS_CC);
	}
	return 0;
}