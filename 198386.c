http1_splitline(struct http *hp, struct http_conn *htc, const int *hf,
    unsigned maxhdr)
{
	char *p, *q;
	int i;

	assert(hf == HTTP1_Req || hf == HTTP1_Resp);
	CHECK_OBJ_NOTNULL(htc, HTTP_CONN_MAGIC);
	CHECK_OBJ_NOTNULL(hp, HTTP_MAGIC);
	assert(htc->rxbuf_e >= htc->rxbuf_b);

	AZ(hp->hd[hf[0]].b);
	AZ(hp->hd[hf[1]].b);
	AZ(hp->hd[hf[2]].b);

	/* Skip leading LWS */
	for (p = htc->rxbuf_b ; vct_islws(*p); p++)
		continue;
	hp->hd[hf[0]].b = p;

	/* First field cannot contain SP or CTL */
	for (; !vct_issp(*p); p++) {
		if (vct_isctl(*p))
			return (400);
	}
	hp->hd[hf[0]].e = p;
	assert(Tlen(hp->hd[hf[0]]));
	*p++ = '\0';

	/* Skip SP */
	for (; vct_issp(*p); p++) {
		if (vct_isctl(*p))
			return (400);
	}
	hp->hd[hf[1]].b = p;

	/* Second field cannot contain LWS or CTL */
	for (; !vct_islws(*p); p++) {
		if (vct_isctl(*p))
			return (400);
	}
	hp->hd[hf[1]].e = p;
	if (!Tlen(hp->hd[hf[1]]))
		return (400);

	/* Skip SP */
	q = p;
	for (; vct_issp(*p); p++) {
		if (vct_isctl(*p))
			return (400);
	}
	if (q < p)
		*q = '\0';	/* Nul guard for the 2nd field. If q == p
				 * (the third optional field is not
				 * present), the last nul guard will
				 * cover this field. */

	/* Third field is optional and cannot contain CTL except TAB */
	q = p;
	for (; p < htc->rxbuf_e && !vct_iscrlf(p, htc->rxbuf_e); p++) {
		if (vct_isctl(*p) && !vct_issp(*p))
			return (400);
	}
	if (p > q) {
		hp->hd[hf[2]].b = q;
		hp->hd[hf[2]].e = p;
	}

	/* Skip CRLF */
	i = vct_iscrlf(p, htc->rxbuf_e);
	if (!i)
		return (400);
	*p = '\0';
	p += i;

	http_Proto(hp);

	return (http1_dissect_hdrs(hp, p, htc, maxhdr));
}