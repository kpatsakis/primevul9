const gchar *conv_get_current_locale(void)
{
	const gchar *cur_locale;

#ifdef G_OS_WIN32
	cur_locale = g_win32_getlocale();
#else
	cur_locale = g_getenv("LC_ALL");
	if (!cur_locale) cur_locale = g_getenv("LC_CTYPE");
	if (!cur_locale) cur_locale = g_getenv("LANG");
	if (!cur_locale) cur_locale = setlocale(LC_CTYPE, NULL);
#endif /* G_OS_WIN32 */

	debug_print("current locale: %s\n",
		    cur_locale ? cur_locale : "(none)");

	return cur_locale;
}