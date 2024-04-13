http1_WrTxt(const struct worker *wrk, const txt *hh, const char *suf)
{
	unsigned u;

	CHECK_OBJ_NOTNULL(wrk, WORKER_MAGIC);
	AN(wrk);
	AN(hh);
	AN(hh->b);
	AN(hh->e);
	u = V1L_Write(wrk, hh->b, hh->e - hh->b);
	if (suf != NULL)
		u += V1L_Write(wrk, suf, -1);
	return (u);
}