HTTP1_DissectRequest(struct http_conn *htc, struct http *hp)
{
	uint16_t retval;
	const char *p;
	const char *b = NULL, *e;

	CHECK_OBJ_NOTNULL(htc, HTTP_CONN_MAGIC);
	CHECK_OBJ_NOTNULL(hp, HTTP_MAGIC);

	retval = http1_splitline(hp, htc,
	    HTTP1_Req, cache_param->http_req_hdr_len);
	if (retval != 0)
		return (retval);

	if (hp->protover < 10 || hp->protover > 11)
		return (400);

	if (http_CountHdr(hp, H_Host) > 1)
		return (400);

	if (http_CountHdr(hp, H_Content_Length) > 1)
		return (400);

	/* RFC2616, section 5.2, point 1 */
	if (!strncasecmp(hp->hd[HTTP_HDR_URL].b, "http://", 7))
		b = hp->hd[HTTP_HDR_URL].b + 7;
	else if (FEATURE(FEATURE_HTTPS_SCHEME) &&
	    !strncasecmp(hp->hd[HTTP_HDR_URL].b, "https://", 8))
		b = hp->hd[HTTP_HDR_URL].b + 8;
	if (b) {
		e = strchr(b, '/');
		if (e) {
			http_Unset(hp, H_Host);
			http_PrintfHeader(hp, "Host: %.*s", (int)(e - b), b);
			hp->hd[HTTP_HDR_URL].b = e;
		}
	}

	htc->body_status = http1_body_status(hp, htc, 1);
	if (htc->body_status == BS_ERROR)
		return (400);

	p = http_GetMethod(hp);
	AN(p);

	if (htc->body_status == BS_EOF) {
		assert(hp->protover == 10);
		/* RFC1945 8.3 p32 and D.1.1 p58 */
		if (!strcasecmp(p, "post") || !strcasecmp(p, "put"))
			return (400);
		htc->body_status = BS_NONE;
	}

	/* HEAD with a body is a hard error */
	if (htc->body_status != BS_NONE && !strcasecmp(p, "head"))
		return (400);

	return (retval);
}