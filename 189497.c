e_util_debug_printv (const gchar *domain,
		     const gchar *format,
		     va_list args)
{
	GString *str;
	GDateTime *dt;

	if (!domain)
		domain = "???";

	str = g_string_new ("");
	g_string_vprintf (str, format, args);
	dt = g_date_time_new_now_local ();

	if (dt) {
		g_print ("[%s] %04d-%02d-%02d %02d:%02d:%02d.%03d - %s",
			domain,
			g_date_time_get_year (dt),
			g_date_time_get_month (dt),
			g_date_time_get_day_of_month (dt),
			g_date_time_get_hour (dt),
			g_date_time_get_minute (dt),
			g_date_time_get_second (dt),
			g_date_time_get_microsecond (dt) / 1000,
			str->str);
		g_date_time_unref (dt);
	} else {
		g_print ("[%s] %s", domain, str->str);
	}

	g_string_free (str, TRUE);
}