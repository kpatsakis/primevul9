ignorableWhitespaceDebug(void *ctx ATTRIBUTE_UNUSED, const xmlChar *ch, int len)
{
    char out[40];
    int i;

    callbacks++;
    if (noout)
	return;
    for (i = 0;(i<len) && (i < 30);i++)
	out[i] = ch[i];
    out[i] = 0;
    fprintf(stdout, "SAX.ignorableWhitespace(%s, %d)\n", out, len);
}