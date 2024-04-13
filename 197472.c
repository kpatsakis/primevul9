ZEND_API int _object_and_properties_init(zval *arg, zend_class_entry *class_type, HashTable *properties ZEND_FILE_LINE_DC TSRMLS_DC) /* {{{ */
{
	zend_object *object;

	if (class_type->ce_flags & (ZEND_ACC_INTERFACE|ZEND_ACC_IMPLICIT_ABSTRACT_CLASS|ZEND_ACC_EXPLICIT_ABSTRACT_CLASS)) {
		char *what =   (class_type->ce_flags & ZEND_ACC_INTERFACE)                ? "interface"
					 :((class_type->ce_flags & ZEND_ACC_TRAIT) == ZEND_ACC_TRAIT) ? "trait"
					 :                                                              "abstract class";
		zend_error(E_ERROR, "Cannot instantiate %s %s", what, class_type->name);
	}

	zend_update_class_constants(class_type TSRMLS_CC);

	Z_TYPE_P(arg) = IS_OBJECT;
	if (class_type->create_object == NULL) {
		Z_OBJVAL_P(arg) = zend_objects_new(&object, class_type TSRMLS_CC);
		if (properties) {
			object->properties = properties;
			object->properties_table = NULL;
		} else {
			object_properties_init(object, class_type);
		}
	} else {
		Z_OBJVAL_P(arg) = class_type->create_object(class_type TSRMLS_CC);
	}
	return SUCCESS;
}