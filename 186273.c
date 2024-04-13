static int php_session_decode(zend_string *data) /* {{{ */
{
	if (!PS(serializer)) {
		php_error_docref(NULL, E_WARNING, "Unknown session.serialize_handler. Failed to decode session object");
		return FAILURE;
	}
	if (PS(serializer)->decode(ZSTR_VAL(data), ZSTR_LEN(data)) == FAILURE) {
		php_session_destroy();
		php_session_track_init();
		php_error_docref(NULL, E_WARNING, "Failed to decode session object. Session has been destroyed");
		return FAILURE;
	}
	return SUCCESS;
}