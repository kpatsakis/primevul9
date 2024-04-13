startDocumentDebug(void *ctx ATTRIBUTE_UNUSED)
{
    callbacks++;
    if (noout)
	return;
    fprintf(stdout, "SAX.startDocument()\n");
}