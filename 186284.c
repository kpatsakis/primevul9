static zend_string *php_session_encode(void) /* {{{ */
{
	IF_SESSION_VARS() {
		if (!PS(serializer)) {
			php_error_docref(NULL, E_WARNING, "Unknown session.serialize_handler. Failed to encode session object");
			return NULL;
		}
		return PS(serializer)->encode();
	} else {
		php_error_docref(NULL, E_WARNING, "Cannot encode non-existent session");
	}
	return NULL;
}