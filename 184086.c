die(int sig)
{
	char buf[256];

	DBGPRINTF("exiting on signal %d\n", sig);

	/* IMPORTANT: we should close the inputs first, and THEN send our termination
	 * message. If we do it the other way around, logmsgInternal() may block on
	 * a full queue and the inputs still fill up that queue. Depending on the
	 * scheduling order, we may end up with logmsgInternal being held for a quite
	 * long time. When the inputs are terminated first, that should not happen
	 * because the queue is drained in parallel. The situation could only become
	 * an issue with extremely long running actions in a queue full environment.
	 * However, such actions are at least considered poorly written, if not
	 * outright wrong. So we do not care about this very remote problem.
	 * rgerhards, 2008-01-11
	 */

	/* close the inputs */
	DBGPRINTF("Terminating input threads...\n");
	thrdTerminateAll();

	/* and THEN send the termination log message (see long comment above) */
	if (sig) {
		(void) snprintf(buf, sizeof(buf) / sizeof(char),
		 " [origin software=\"rsyslogd\" " "swVersion=\"" VERSION \
		 "\" x-pid=\"%d\" x-info=\"http://www.rsyslog.com\"]" " exiting on signal %d.",
		 (int) myPid, sig);
		errno = 0;
		logmsgInternal(NO_ERRCODE, LOG_SYSLOG|LOG_INFO, (uchar*)buf, 0);
	}
	
	/* drain queue (if configured so) and stop main queue worker thread pool */
	DBGPRINTF("Terminating main queue...\n");
	qqueueDestruct(&pMsgQueue);
	pMsgQueue = NULL;

	/* Free ressources and close connections. This includes flushing any remaining
	 * repeated msgs.
	 */
	DBGPRINTF("Terminating outputs...\n");
	destructAllActions();

	DBGPRINTF("all primary multi-thread sources have been terminated - now doing aux cleanup...\n");
	/* rger 2005-02-22
	 * now clean up the in-memory structures. OK, the OS
	 * would also take care of that, but if we do it
	 * ourselfs, this makes finding memory leaks a lot
	 * easier.
	 */
	tplDeleteAll();

	remove_pid(PidFile);

	/* de-init some modules */
	modExitIminternal();

	/*dbgPrintAllDebugInfo(); / * this is the last spot where this can be done - below output modules are unloaded! */

	/* the following line cleans up CfSysLineHandlers that were not based on loadable
	 * modules. As such, they are not yet cleared.
	 */
	unregCfSysLineHdlrs();

	legacyOptsFree();

	/* destruct our global properties */
	if(pInternalInputName != NULL)
		prop.Destruct(&pInternalInputName);
	if(pLocalHostIP != NULL)
		prop.Destruct(&pLocalHostIP);

	/* terminate the remaining classes */
	GlobalClassExit();

	/* TODO: this would also be the right place to de-init the builtin output modules. We
	 * do not currently do that, because the module interface does not allow for
	 * it. This will come some time later (it's essential with loadable modules).
	 * For the time being, this is a memory leak on exit, but as the process is
	 * terminated, we do not really bother about it.
	 * rgerhards, 2007-08-03
	 * I have added some code now, but all that mod init/de-init should be moved to
	 * init, so that modules are unloaded and reloaded on HUP to. Eventually it should go
	 * into destructAllActions() - but that needs to be seen. -- rgerhards, 2007-08-09
	 */
	module.UnloadAndDestructAll(eMOD_LINK_ALL);

	DBGPRINTF("Clean shutdown completed, bye\n");
	/* dbgClassExit MUST be the last one, because it de-inits the debug system */
	dbgClassExit();

	/* free all remaining memory blocks - this is not absolutely necessary, but helps
	 * us keep memory debugger logs clean and this is in aid in developing. It doesn't
	 * cost much time, so we do it always. -- rgerhards, 2008-03-20
	 */
	freeAllDynMemForTermination();
	/* NO CODE HERE - feeelAllDynMemForTermination() must be the last thing before exit()! */
	exit(0); /* "good" exit, this is the terminator function for rsyslog [die()] */
}