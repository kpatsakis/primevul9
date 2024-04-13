char *textpri(char *pRes, size_t pResLen, int pri)
{
	assert(pRes != NULL);
	assert(pResLen > 0);

	snprintf(pRes, pResLen, "%s.%s<%d>", syslog_fac_names[LOG_FAC(pri)],
		 syslog_severity_names[LOG_PRI(pri)], pri);

	return pRes;
}