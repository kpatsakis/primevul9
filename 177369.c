PHP_FUNCTION(mb_ereg_search_getpos)
{
	RETVAL_LONG(MBREX(search_pos));
}