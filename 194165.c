rsRetVal msgConstructWithTime(msg_t **ppThis, struct syslogTime *stTime, time_t ttGenTime)
{
	DEFiRet;

	CHKiRet(msgBaseConstruct(ppThis));
	(*ppThis)->ttGenTime = ttGenTime;
	memcpy(&(*ppThis)->tRcvdAt, stTime, sizeof(struct syslogTime));
	memcpy(&(*ppThis)->tTIMESTAMP, stTime, sizeof(struct syslogTime));

finalize_it:
	RETiRet;
}