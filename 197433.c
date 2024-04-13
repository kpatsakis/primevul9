file_regfree(file_regex_t *rx)
{
	if (rx->rc == 0)
		regfree(&rx->rx);
#ifdef USE_C_LOCALE
	(void)uselocale(rx->old_lc_ctype);
	freelocale(rx->c_lc_ctype);
#endif
}