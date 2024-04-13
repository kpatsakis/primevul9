static gchar *conv_iconv_strdup(const gchar *inbuf,
			 const gchar *src_code, const gchar *dest_code)
{
	iconv_t cd;
	gchar *outbuf;

	if (!src_code && !dest_code && 
	    g_utf8_validate(inbuf, -1, NULL))
	    	return g_strdup(inbuf);

	if (!src_code)
		src_code = conv_get_outgoing_charset_str();
	if (!dest_code)
		dest_code = CS_INTERNAL;

	/* don't convert if src and dest codeset are identical */
	if (!strcasecmp(src_code, dest_code))
		return g_strdup(inbuf);

	/* don't convert if dest codeset is US-ASCII */
	if (!strcasecmp(src_code, CS_US_ASCII))
		return g_strdup(inbuf);

	/* don't convert if dest codeset is US-ASCII */
	if (!strcasecmp(dest_code, CS_US_ASCII))
		return g_strdup(inbuf);

	cd = iconv_open(dest_code, src_code);
	if (cd == (iconv_t)-1)
		return NULL;

	outbuf = conv_iconv_strdup_with_cd(inbuf, cd);

	iconv_close(cd);

	return outbuf;
}