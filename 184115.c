doHUP(void)
{
	char buf[512];

	snprintf(buf, sizeof(buf) / sizeof(char),
		 " [origin software=\"rsyslogd\" " "swVersion=\"" VERSION
		 "\" x-pid=\"%d\" x-info=\"http://www.rsyslog.com\"] rsyslogd was HUPed, type '%s'.",
		 (int) myPid, glbl.GetHUPisRestart() ? "restart" : "lightweight");
		errno = 0;
	logmsgInternal(NO_ERRCODE, LOG_SYSLOG|LOG_INFO, (uchar*)buf, 0);

	if(glbl.GetHUPisRestart()) {
		DBGPRINTF("Received SIGHUP, configured to be restart, reloading rsyslogd.\n");
		init(); /* main queue is stopped as part of init() */
		runInputModules();
	} else {
		DBGPRINTF("Received SIGHUP, configured to be a non-restart type of HUP - notifying actions.\n");
		ruleset.IterateAllActions(doHUPActions, NULL);
	}
}