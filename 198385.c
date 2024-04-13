HTTP1_DissectResponse(struct http_conn *htc, struct http *hp,
    const struct http *rhttp)
{
	uint16_t retval = 0;
	const char *p;

	CHECK_OBJ_NOTNULL(htc, HTTP_CONN_MAGIC);
	CHECK_OBJ_NOTNULL(hp, HTTP_MAGIC);
	CHECK_OBJ_NOTNULL(rhttp, HTTP_MAGIC);

	if (http1_splitline(hp, htc,
	    HTTP1_Resp, cache_param->http_resp_hdr_len))
		retval = 503;

	if (retval == 0 && hp->protover < 10)
		retval = 503;

	if (retval == 0 && hp->protover > rhttp->protover)
		http_SetH(hp, HTTP_HDR_PROTO, rhttp->hd[HTTP_HDR_PROTO].b);

	if (retval == 0 && Tlen(hp->hd[HTTP_HDR_STATUS]) != 3)
		retval = 503;

	if (retval == 0) {
		p = hp->hd[HTTP_HDR_STATUS].b;

		if (p[0] >= '1' && p[0] <= '9' &&
		    p[1] >= '0' && p[1] <= '9' &&
		    p[2] >= '0' && p[2] <= '9')
			hp->status =
			    100 * (p[0] - '0') + 10 * (p[1] - '0') + p[2] - '0';
		else
			retval = 503;
	}

	if (retval != 0) {
		VSLb(hp->vsl, SLT_HttpGarbage, "%.*s",
		    (int)(htc->rxbuf_e - htc->rxbuf_b), htc->rxbuf_b);
		assert(retval >= 100 && retval <= 999);
		assert(retval == 503);
		http_SetStatus(hp, 503);
	}

	if (hp->hd[HTTP_HDR_REASON].b == NULL ||
	    !Tlen(hp->hd[HTTP_HDR_REASON])) {
		http_SetH(hp, HTTP_HDR_REASON,
		    http_Status2Reason(hp->status, NULL));
	}

	htc->body_status = http1_body_status(hp, htc, 0);

	return (retval);
}