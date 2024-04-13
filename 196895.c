referenceDebug(void *ctx ATTRIBUTE_UNUSED, const xmlChar *name)
{
    callbacks++;
    if (noout)
	return;
    fprintf(stdout, "SAX.reference(%s)\n", name);
}