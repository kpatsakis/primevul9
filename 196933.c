cdataBlockDebug(void *ctx ATTRIBUTE_UNUSED, const xmlChar *value, int len)
{
    callbacks++;
    if (noout)
	return;
    fprintf(stdout, "SAX.pcdata(%.20s, %d)\n",
	    (char *) value, len);
}