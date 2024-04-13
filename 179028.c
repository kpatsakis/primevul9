
static zval* date_clone_immutable(zval *object TSRMLS_DC)
{
	zval *new_object;

	ALLOC_ZVAL(new_object);
	Z_OBJVAL_P(new_object) = date_object_clone_date(object TSRMLS_CC);
	Z_SET_REFCOUNT_P(new_object, 1);
	Z_SET_ISREF_P(new_object);
	Z_TYPE_P(new_object) = IS_OBJECT;

	return new_object;