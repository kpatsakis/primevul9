get_zonename(struct tm *tmp)
{
#if defined(_WIN32)
	/*
	 * The strings in _tzname[] are encoded using the code page
	 * for the current C-language locale.
	 *
	 * On Windows, all Wireshark programs set that code page
	 * to the UTF-8 code page by calling
	 *
	 *	  setlocale(LC_ALL, ".UTF-8");
	 *
	 * so the strings in _tzname[] are UTF-8 strings, and we can
	 * just return them.
	 *
	 * (Note: the above does *not* mean we've set any code pages
	 * *other* than the one used by the Visual Studio C runtime
	 * to UTF-8, so don't assume, for example, that the "ANSI"
	 * versions of Windows APIs will take UTF-8 strings, or that
	 * non-UTF-16 output to the console will be treated as UTF-8.
	 * Setting those other code pages can cause problems, especially
	 * on pre-Windows 10 or older Windows 10 releases.)
	 */
	return _tzname[tmp->tm_isdst];
#else
	/*
	 * UN*X.
	 *
	 * If we have tm_zone in struct tm, use that.
	 * Otherwise, if we have tzname[], use it, otherwise just
	 * say "we don't know.
	 */
# if defined(HAVE_STRUCT_TM_TM_ZONE)
	return tmp->tm_zone;
# else /* HAVE_STRUCT_TM_TM_ZONE */
	if ((tmp->tm_isdst != 0) && (tmp->tm_isdst != 1)) {
		return "???";
	}
#  if defined(HAVE_TZNAME)
	return tzname[tmp->tm_isdst];
#  else
	return tmp->tm_isdst ? "?DT" : "?ST";
#  endif /* HAVE_TZNAME */
# endif /* HAVE_STRUCT_TM_TM_ZONE */
#endif /* _WIN32 */
}