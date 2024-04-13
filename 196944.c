processingInstructionDebug(void *ctx ATTRIBUTE_UNUSED, const xmlChar *target,
                      const xmlChar *data)
{
    callbacks++;
    if (noout)
	return;
    if (data != NULL)
	fprintf(stdout, "SAX.processingInstruction(%s, %s)\n",
		(char *) target, (char *) data);
    else
	fprintf(stdout, "SAX.processingInstruction(%s, NULL)\n",
		(char *) target);
}