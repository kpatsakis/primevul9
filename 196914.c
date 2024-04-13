getParameterEntityDebug(void *ctx ATTRIBUTE_UNUSED, const xmlChar *name)
{
    callbacks++;
    if (noout)
	return(NULL);
    fprintf(stdout, "SAX.getParameterEntity(%s)\n", name);
    return(NULL);
}