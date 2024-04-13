HTTP1_Write(const struct worker *w, const struct http *hp, const int *hf)
{
	unsigned u, l;

	assert(hf == HTTP1_Req || hf == HTTP1_Resp);
	AN(hp->hd[hf[0]].b);
	AN(hp->hd[hf[1]].b);
	AN(hp->hd[hf[2]].b);
	l = http1_WrTxt(w, &hp->hd[hf[0]], " ");
	l += http1_WrTxt(w, &hp->hd[hf[1]], " ");
	l += http1_WrTxt(w, &hp->hd[hf[2]], "\r\n");

	for (u = HTTP_HDR_FIRST; u < hp->nhd; u++)
		l += http1_WrTxt(w, &hp->hd[u], "\r\n");
	l += V1L_Write(w, "\r\n", -1);
	return (l);
}