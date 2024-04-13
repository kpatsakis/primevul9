static uchar *getNOW(eNOWType eNow)
{
	uchar *pBuf;
	struct syslogTime t;

	if((pBuf = (uchar*) MALLOC(sizeof(uchar) * tmpBUFSIZE)) == NULL) {
		return NULL;
	}

	datetime.getCurrTime(&t, NULL);
	switch(eNow) {
	case NOW_NOW:
		snprintf((char*) pBuf, tmpBUFSIZE, "%4.4d-%2.2d-%2.2d", t.year, t.month, t.day);
		break;
	case NOW_YEAR:
		snprintf((char*) pBuf, tmpBUFSIZE, "%4.4d", t.year);
		break;
	case NOW_MONTH:
		snprintf((char*) pBuf, tmpBUFSIZE, "%2.2d", t.month);
		break;
	case NOW_DAY:
		snprintf((char*) pBuf, tmpBUFSIZE, "%2.2d", t.day);
		break;
	case NOW_HOUR:
		snprintf((char*) pBuf, tmpBUFSIZE, "%2.2d", t.hour);
		break;
	case NOW_HHOUR:
		snprintf((char*) pBuf, tmpBUFSIZE, "%2.2d", t.minute / 30);
		break;
	case NOW_QHOUR:
		snprintf((char*) pBuf, tmpBUFSIZE, "%2.2d", t.minute / 15);
		break;
	case NOW_MINUTE:
		snprintf((char*) pBuf, tmpBUFSIZE, "%2.2d", t.minute);
		break;
	}

	return(pBuf);
}