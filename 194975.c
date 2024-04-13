PHP_FUNCTION(gethostname)
{
	char buf[HOST_NAME_MAX + 1];

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (gethostname(buf, sizeof(buf))) {
		php_error_docref(NULL, E_WARNING, "unable to fetch host [%d]: %s", errno, strerror(errno));
		RETURN_FALSE;
	}

	RETURN_STRING(buf);
}