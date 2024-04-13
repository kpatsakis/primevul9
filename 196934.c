startElementDebug(void *ctx ATTRIBUTE_UNUSED, const xmlChar *name, const xmlChar **atts)
{
    int i;

    callbacks++;
    if (noout)
	return;
    fprintf(stdout, "SAX.startElement(%s", (char *) name);
    if (atts != NULL) {
        for (i = 0;(atts[i] != NULL);i++) {
	    fprintf(stdout, ", %s='", atts[i++]);
	    if (atts[i] != NULL)
	        fprintf(stdout, "%s'", atts[i]);
	}
    }
    fprintf(stdout, ")\n");
}