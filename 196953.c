commentDebug(void *ctx ATTRIBUTE_UNUSED, const xmlChar *value)
{
    callbacks++;
    if (noout)
	return;
    fprintf(stdout, "SAX.comment(%s)\n", value);
}