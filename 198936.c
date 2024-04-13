PHP_RINIT_FUNCTION(mb_regex)
{
	return MBSTRG(mb_regex_globals) ? SUCCESS: FAILURE;
}