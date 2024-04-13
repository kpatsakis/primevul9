static rsRetVal mainThread()
{
	DEFiRet;
	uchar *pTmp;

	/* Note: signals MUST be processed by the thread this code is running in. The reason
	 * is that we need to interrupt the select() system call. -- rgerhards, 2007-10-17
	 */

	/* initialize the build-in templates */
	pTmp = template_DebugFormat;
	tplAddLine("RSYSLOG_DebugFormat", &pTmp);
	pTmp = template_SyslogProtocol23Format;
	tplAddLine("RSYSLOG_SyslogProtocol23Format", &pTmp);
	pTmp = template_FileFormat; /* new format for files with high-precision stamp */
	tplAddLine("RSYSLOG_FileFormat", &pTmp);
	pTmp = template_TraditionalFileFormat;
	tplAddLine("RSYSLOG_TraditionalFileFormat", &pTmp);
	pTmp = template_WallFmt;
	tplAddLine(" WallFmt", &pTmp);
	pTmp = template_ForwardFormat;
	tplAddLine("RSYSLOG_ForwardFormat", &pTmp);
	pTmp = template_TraditionalForwardFormat;
	tplAddLine("RSYSLOG_TraditionalForwardFormat", &pTmp);
	pTmp = template_StdUsrMsgFmt;
	tplAddLine(" StdUsrMsgFmt", &pTmp);
	pTmp = template_StdDBFmt;
	tplAddLine(" StdDBFmt", &pTmp);
        pTmp = template_StdPgSQLFmt;
        tplLastStaticInit(tplAddLine(" StdPgSQLFmt", &pTmp));

	CHKiRet(init());

	if(Debug && debugging_on) {
		DBGPRINTF("Debugging enabled, SIGUSR1 to turn off debugging.\n");
	}

	/* Send a signal to the parent so it can terminate.
	 */
	if(myPid != ppid)
		kill(ppid, SIGTERM);


	/* If instructed to do so, we now drop privileges. Note that this is not 100% secure,
	 * because outputs are already running at this time. However, we can implement
	 * dropping of privileges rather quickly and it will work in many cases. While it is not
	 * the ultimate solution, the current one is still much better than not being able to
	 * drop privileges at all. Doing it correctly, requires a change in architecture, which
	 * we should do over time. TODO -- rgerhards, 2008-11-19
	 */
	if(gidDropPriv != 0) {
		doDropPrivGid(gidDropPriv);
		glbl.SetHUPisRestart(0); /* we can not do restart-type HUPs with dropped privs */
	}

	if(uidDropPriv != 0) {
		doDropPrivUid(uidDropPriv);
		glbl.SetHUPisRestart(0); /* we can not do restart-type HUPs with dropped privs */
	}

	/* finally let the inputs run... */
	runInputModules();

	/* END OF INTIALIZATION
	 * ... but keep in mind that we might do a restart and thus init() might
	 * be called again. -- rgerhards, 2005-10-24
	 */
	DBGPRINTF("initialization completed, transitioning to regular run mode\n");

	/* close stderr and stdout if they are kept open during a fork. Note that this
	 * may introduce subtle security issues: if we are in a jail, one may break out of
	 * it via these descriptors. But if I close them earlier, error messages will (once
	 * again) not be emitted to the user that starts the daemon. As root jail support
	 * is still in its infancy (and not really done), we currently accept this issue.
	 * rgerhards, 2009-06-29
	 */
	if(!(Debug || NoFork)) {
		close(1);
		close(2);
		bErrMsgToStderr = 0;
	}

	mainloop();

finalize_it:
	RETiRet;
}