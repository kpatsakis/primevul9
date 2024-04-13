elementDeclDebug(void *ctx ATTRIBUTE_UNUSED, const xmlChar *name, int type,
	    xmlElementContentPtr content ATTRIBUTE_UNUSED)
{
    callbacks++;
    if (noout)
	return;
    fprintf(stdout, "SAX.elementDecl(%s, %d, ...)\n",
            name, type);
}