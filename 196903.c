isStandaloneDebug(void *ctx ATTRIBUTE_UNUSED)
{
    callbacks++;
    if (noout)
	return(0);
    fprintf(stdout, "SAX.isStandalone()\n");
    return(0);
}