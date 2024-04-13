endElementDebug(void *ctx ATTRIBUTE_UNUSED, const xmlChar *name)
{
    callbacks++;
    if (noout)
	return;
    fprintf(stdout, "SAX.endElement(%s)\n", (char *) name);
}