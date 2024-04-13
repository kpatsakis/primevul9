static PHP_RINIT_FUNCTION(session) /* {{{ */
{
	return php_rinit_session(PS(auto_start));
}