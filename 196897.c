notationDeclDebug(void *ctx ATTRIBUTE_UNUSED, const xmlChar *name,
	     const xmlChar *publicId, const xmlChar *systemId)
{
    callbacks++;
    if (noout)
	return;
    fprintf(stdout, "SAX.notationDecl(%s, %s, %s)\n",
            (char *) name, (char *) publicId, (char *) systemId);
}