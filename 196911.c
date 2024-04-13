hasInternalSubsetDebug(void *ctx ATTRIBUTE_UNUSED)
{
    callbacks++;
    if (noout)
	return(0);
    fprintf(stdout, "SAX.hasInternalSubset()\n");
    return(0);
}