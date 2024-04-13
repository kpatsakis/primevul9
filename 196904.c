hasExternalSubsetDebug(void *ctx ATTRIBUTE_UNUSED)
{
    callbacks++;
    if (noout)
	return(0);
    fprintf(stdout, "SAX.hasExternalSubset()\n");
    return(0);
}