static rsRetVal resetConfigVariables(uchar __attribute__((unused)) *pp, void __attribute__((unused)) *pVal)
{
	cCCEscapeChar = '#';
	bActExecWhenPrevSusp = 0;
	iActExecOnceInterval = 0;
	bDebugPrintTemplateList = 1;
	bDebugPrintCfSysLineHandlerList = 1;
	bDebugPrintModuleList = 1;
	bEscapeCCOnRcv = 1; /* default is to escape control characters */
	bReduceRepeatMsgs = 0;
	free(pszMainMsgQFName);
	pszMainMsgQFName = NULL;
	iMainMsgQueueSize = 10000;
	iMainMsgQHighWtrMark = 8000;
	iMainMsgQLowWtrMark = 2000;
	iMainMsgQDiscardMark = 9800;
	iMainMsgQDiscardSeverity = 8;
	iMainMsgQueMaxFileSize = 1024 * 1024;
	iMainMsgQueueNumWorkers = 1;
	iMainMsgQPersistUpdCnt = 0;
	bMainMsgQSyncQeueFiles = 0;
	iMainMsgQtoQShutdown = 1500;
	iMainMsgQtoActShutdown = 1000;
	iMainMsgQtoEnq = 2000;
	iMainMsgQtoWrkShutdown = 60000;
	iMainMsgQWrkMinMsgs = 100;
	iMainMsgQDeqSlowdown = 0;
	bMainMsgQSaveOnShutdown = 1;
	MainMsgQueType = QUEUETYPE_FIXED_ARRAY;
	iMainMsgQueMaxDiskSpace = 0;
	glbliActionResumeRetryCount = 0;

	return RS_RET_OK;
}