static void doDropPrivUid(int iUid)
{
	int res;
	uchar szBuf[1024];

	res = setuid(iUid);
	if(res) {
		/* if we can not set the userid, this is fatal, so let's unconditionally abort */
		perror("could not set requested userid");
		exit(1);
	}
	DBGPRINTF("setuid(%d): %d\n", iUid, res);
	snprintf((char*)szBuf, sizeof(szBuf)/sizeof(uchar), "rsyslogd's userid changed to %d", iUid);
	logmsgInternal(NO_ERRCODE, LOG_SYSLOG|LOG_INFO, szBuf, 0);
}