warningDebug(void *ctx ATTRIBUTE_UNUSED, const char *msg, ...)
{
    va_list args;

    callbacks++;
    if (noout)
	return;
    va_start(args, msg);
    fprintf(stdout, "SAX.warning: ");
    vfprintf(stdout, msg, args);
    va_end(args);
}