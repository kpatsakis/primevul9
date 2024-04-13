abs_time_secs_to_str(wmem_allocator_t *scope, const time_t abs_time, const absolute_time_display_e fmt,
		gboolean show_zone)
{
	struct tm *tmp = NULL;
	const char *zonename = "???";
	gchar *buf = NULL;

	switch (fmt) {

		case ABSOLUTE_TIME_UTC:
		case ABSOLUTE_TIME_DOY_UTC:
		case ABSOLUTE_TIME_NTP_UTC:
			tmp = gmtime(&abs_time);
			zonename = "UTC";
			break;

		case ABSOLUTE_TIME_LOCAL:
			tmp = localtime(&abs_time);
			if (tmp) {
				zonename = get_zonename(tmp);
			}
			break;
	}
	if (tmp) {
		switch (fmt) {

			case ABSOLUTE_TIME_DOY_UTC:
				if (show_zone) {
					buf = wmem_strdup_printf(scope,
							"%04d/%03d:%02d:%02d:%02d %s",
							tmp->tm_year + 1900,
							tmp->tm_yday + 1,
							tmp->tm_hour,
							tmp->tm_min,
							tmp->tm_sec,
							zonename);
				} else {
					buf = wmem_strdup_printf(scope,
							"%04d/%03d:%02d:%02d:%02d",
							tmp->tm_year + 1900,
							tmp->tm_yday + 1,
							tmp->tm_hour,
							tmp->tm_min,
							tmp->tm_sec);
				}
				break;

			case ABSOLUTE_TIME_NTP_UTC:
				if (abs_time == 0) {
					buf = wmem_strdup(scope, "NULL");
					break;
				}
				/* FALLTHROUGH */
			case ABSOLUTE_TIME_UTC:
			case ABSOLUTE_TIME_LOCAL:
				if (show_zone) {
					buf = wmem_strdup_printf(scope,
							"%s %2d, %d %02d:%02d:%02d %s",
							mon_names[tmp->tm_mon],
							tmp->tm_mday,
							tmp->tm_year + 1900,
							tmp->tm_hour,
							tmp->tm_min,
							tmp->tm_sec,
							zonename);
				} else {
					buf = wmem_strdup_printf(scope,
							"%s %2d, %d %02d:%02d:%02d",
							mon_names[tmp->tm_mon],
							tmp->tm_mday,
							tmp->tm_year + 1900,
							tmp->tm_hour,
							tmp->tm_min,
							tmp->tm_sec);
				}
				break;
		}
	} else
		buf = wmem_strdup(scope, "Not representable");
	return buf;
}