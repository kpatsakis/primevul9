static void doDropPrivGid(int iGid)
{
	int res;
	uchar szBuf[1024];

	res = setgroups(0, NULL); /* remove all supplementary group IDs */
	if(res) {
		perror("could not remove supplemental group IDs");
		exit(1);
	}
	DBGPRINTF("setgroups(0, NULL): %d\n", res);
	res = setgid(iGid);
	if(res) {
		/* if we can not set the userid, this is fatal, so let's unconditionally abort */
		perror("could not set requested group id");
		exit(1);
	}
	DBGPRINTF("setgid(%d): %d\n", iGid, res);
	snprintf((char*)szBuf, sizeof(szBuf)/sizeof(uchar), "rsyslogd's groupid changed to %d", iGid);
	logmsgInternal(NO_ERRCODE, LOG_SYSLOG|LOG_INFO, szBuf, 0);
}