util_fatal(const char *fmt, ...)
{
	va_list	ap;

	va_start(ap, fmt);
	fprintf(stderr, "error: ");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\nAborting.\n");
	va_end(ap);

	sc_notify_close();

	exit(1);
}