unsigned_time_secs_to_str_buf(guint32 time_val, const guint32 frac,
    const gboolean is_nsecs, wmem_strbuf_t *buf)
{
	int hours, mins, secs;
	gboolean do_comma = FALSE;

	secs = time_val % 60;
	time_val /= 60;
	mins = time_val % 60;
	time_val /= 60;
	hours = time_val % 24;
	time_val /= 24;

	if (time_val != 0) {
		wmem_strbuf_append_printf(buf, "%u day%s", time_val, PLURALIZE(time_val));
		do_comma = TRUE;
	}
	if (hours != 0) {
		wmem_strbuf_append_printf(buf, "%s%u hour%s", COMMA(do_comma), hours, PLURALIZE(hours));
		do_comma = TRUE;
	}
	if (mins != 0) {
		wmem_strbuf_append_printf(buf, "%s%u minute%s", COMMA(do_comma), mins, PLURALIZE(mins));
		do_comma = TRUE;
	}
	if (secs != 0 || frac != 0) {
		if (frac != 0) {
			if (is_nsecs)
				wmem_strbuf_append_printf(buf, "%s%u.%09u seconds", COMMA(do_comma), secs, frac);
			else
				wmem_strbuf_append_printf(buf, "%s%u.%03u seconds", COMMA(do_comma), secs, frac);
		} else
			wmem_strbuf_append_printf(buf, "%s%u second%s", COMMA(do_comma), secs, PLURALIZE(secs));
	}
}