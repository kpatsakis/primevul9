setDocumentLocatorDebug(void *ctx ATTRIBUTE_UNUSED, xmlSAXLocatorPtr loc ATTRIBUTE_UNUSED)
{
    callbacks++;
    if (noout)
	return;
    fprintf(stdout, "SAX.setDocumentLocator()\n");
}