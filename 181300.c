file_printf(struct magic_set *ms, const char *fmt, ...)
{
	va_list ap;
	int len;
	char *buf = NULL, *newstr;

	va_start(ap, fmt);
	len = vspprintf(&buf, 0, fmt, ap);
	va_end(ap);

	if (ms->o.buf != NULL) {
		len = spprintf(&newstr, 0, "%s%s", ms->o.buf, (buf ? buf : ""));
		if (buf) {
			efree(buf);
		}
		efree(ms->o.buf);
		ms->o.buf = newstr;
	} else {
		ms->o.buf = buf;
	}
	return 0;
}