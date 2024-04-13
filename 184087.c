submitErrMsg(int iErr, uchar *msg)
{
	DEFiRet;
	iRet = logmsgInternal(iErr, LOG_SYSLOG|LOG_ERR, msg, 0);
	RETiRet;
}