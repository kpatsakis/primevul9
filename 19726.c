cdf_print_elapsed_time(char *buf, size_t bufsiz, cdf_timestamp_t ts)
{
	int len = 0;
	int days, hours, mins, secs;

	ts /= CDF_TIME_PREC;
	secs = CAST(int, ts % 60);
	ts /= 60;
	mins = CAST(int, ts % 60);
	ts /= 60;
	hours = CAST(int, ts % 24);
	ts /= 24;
	days = CAST(int, ts);

	if (days) {
		len += snprintf(buf + len, bufsiz - len, "%dd+", days);
		if (CAST(size_t, len) >= bufsiz)
			return len;
	}

	if (days || hours) {
		len += snprintf(buf + len, bufsiz - len, "%.2d:", hours);
		if (CAST(size_t, len) >= bufsiz)
			return len;
	}

	len += snprintf(buf + len, bufsiz - len, "%.2d:", mins);
	if (CAST(size_t, len) >= bufsiz)
		return len;

	len += snprintf(buf + len, bufsiz - len, "%.2d", secs);
	return len;
}