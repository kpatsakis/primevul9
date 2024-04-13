getEntityDebug(void *ctx ATTRIBUTE_UNUSED, const xmlChar *name)
{
    callbacks++;
    if (noout)
	return(NULL);
    fprintf(stdout, "SAX.getEntity(%s)\n", name);
    return(NULL);
}