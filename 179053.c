
static int implement_date_interface_handler(zend_class_entry *interface, zend_class_entry *implementor TSRMLS_DC)
{
	if (implementor->type == ZEND_USER_CLASS &&
		!instanceof_function(implementor, date_ce_date TSRMLS_CC) &&
		!instanceof_function(implementor, date_ce_immutable TSRMLS_CC)
	) {
		zend_error(E_ERROR, "DateTimeInterface can't be implemented by user classes");
	}

	return SUCCESS;