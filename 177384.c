PHP_MINIT_FUNCTION(mb_regex)
{
	onig_init();
	return SUCCESS;
}