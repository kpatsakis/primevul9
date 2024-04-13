static void dbgPrintInitInfo(void)
{
	ruleset.DebugPrintAll();
	DBGPRINTF("\n");
	if(bDebugPrintTemplateList)
		tplPrintList();
	if(bDebugPrintModuleList)
		module.PrintList();
	ochPrintList();

	if(bDebugPrintCfSysLineHandlerList)
		dbgPrintCfSysLineHandlers();

	DBGPRINTF("Messages with malicious PTR DNS Records are %sdropped.\n",
		  glbl.GetDropMalPTRMsgs() ? "" : "not ");

	DBGPRINTF("Control characters are %sreplaced upon reception.\n",
		  bEscapeCCOnRcv? "" : "not ");

	if(bEscapeCCOnRcv)
		DBGPRINTF("Control character escape sequence prefix is '%c'.\n",
			cCCEscapeChar);

	DBGPRINTF("Main queue size %d messages.\n", iMainMsgQueueSize);
	DBGPRINTF("Main queue worker threads: %d, wThread shutdown: %d, Perists every %d updates.\n",
		  iMainMsgQueueNumWorkers, iMainMsgQtoWrkShutdown, iMainMsgQPersistUpdCnt);
	DBGPRINTF("Main queue timeouts: shutdown: %d, action completion shutdown: %d, enq: %d\n",
		   iMainMsgQtoQShutdown, iMainMsgQtoActShutdown, iMainMsgQtoEnq);
	DBGPRINTF("Main queue watermarks: high: %d, low: %d, discard: %d, discard-severity: %d\n",
		   iMainMsgQHighWtrMark, iMainMsgQLowWtrMark, iMainMsgQDiscardMark, iMainMsgQDiscardSeverity);
	DBGPRINTF("Main queue save on shutdown %d, max disk space allowed %lld\n",
		   bMainMsgQSaveOnShutdown, iMainMsgQueMaxDiskSpace);
	/* TODO: add
	iActionRetryCount = 0;
	iActionRetryInterval = 30000;
	static int iMainMsgQtoWrkMinMsgs = 100;	
	static int iMainMsgQbSaveOnShutdown = 1;
	iMainMsgQueMaxDiskSpace = 0;
	setQPROP(qqueueSetiMinMsgsPerWrkr, "$MainMsgQueueWorkerThreadMinimumMessages", 100);
	setQPROP(qqueueSetbSaveOnShutdown, "$MainMsgQueueSaveOnShutdown", 1);
	 */
	DBGPRINTF("Work Directory: '%s'.\n", glbl.GetWorkDir());
}