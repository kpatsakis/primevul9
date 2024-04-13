static rsRetVal loadBuildInModules(void)
{
	DEFiRet;

	if((iRet = module.doModInit(modInitFile, UCHAR_CONSTANT("builtin-file"), NULL)) != RS_RET_OK) {
		RETiRet;
	}
	if((iRet = module.doModInit(modInitPipe, UCHAR_CONSTANT("builtin-pipe"), NULL)) != RS_RET_OK) {
		RETiRet;
	}
#ifdef SYSLOG_INET
	if((iRet = module.doModInit(modInitFwd, UCHAR_CONSTANT("builtin-fwd"), NULL)) != RS_RET_OK) {
		RETiRet;
	}
#endif
	if((iRet = module.doModInit(modInitShell, UCHAR_CONSTANT("builtin-shell"), NULL)) != RS_RET_OK) {
		RETiRet;
	}
	if((iRet = module.doModInit(modInitDiscard, UCHAR_CONSTANT("builtin-discard"), NULL)) != RS_RET_OK) {
		RETiRet;
	}

	/* dirty, but this must be for the time being: the usrmsg module must always be
	 * loaded as last module. This is because it processes any type of action selector.
	 * If we load it before other modules, these others will never have a chance of
	 * working with the config file. We may change that implementation so that a user name
	 * must start with an alnum, that would definitely help (but would it break backwards
	 * compatibility?). * rgerhards, 2007-07-23
	 * User names now must begin with:
	 *   [a-zA-Z0-9_.]
	 */
	CHKiRet(module.doModInit(modInitUsrMsg, (uchar*) "builtin-usrmsg", NULL));

	/* ok, initialization of the command handler probably does not 100% belong right in
	 * this space here. However, with the current design, this is actually quite a good
	 * place to put it. We might decide to shuffle it around later, but for the time
	 * being, the code has found its home here. A not-just-sideeffect of this decision
	 * is that rsyslog will terminate if we can not register our built-in config commands.
	 * This, I think, is the right thing to do. -- rgerhards, 2007-07-31
	 */
	CHKiRet(regCfSysLineHdlr((uchar *)"actionresumeretrycount", 0, eCmdHdlrInt, NULL, &glbliActionResumeRetryCount, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"defaultruleset", 0, eCmdHdlrGetWord, setDefaultRuleset, NULL, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"ruleset", 0, eCmdHdlrGetWord, setCurrRuleset, NULL, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"mainmsgqueuefilename", 0, eCmdHdlrGetWord, NULL, &pszMainMsgQFName, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"mainmsgqueuesize", 0, eCmdHdlrInt, NULL, &iMainMsgQueueSize, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"mainmsgqueuehighwatermark", 0, eCmdHdlrInt, NULL, &iMainMsgQHighWtrMark, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"mainmsgqueuelowwatermark", 0, eCmdHdlrInt, NULL, &iMainMsgQLowWtrMark, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"mainmsgqueuediscardmark", 0, eCmdHdlrInt, NULL, &iMainMsgQDiscardMark, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"mainmsgqueuediscardseverity", 0, eCmdHdlrSeverity, NULL, &iMainMsgQDiscardSeverity, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"mainmsgqueuecheckpointinterval", 0, eCmdHdlrInt, NULL, &iMainMsgQPersistUpdCnt, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"mainmsgqueuesyncqueuefiles", 0, eCmdHdlrBinary, NULL, &bMainMsgQSyncQeueFiles, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"mainmsgqueuetype", 0, eCmdHdlrGetWord, setMainMsgQueType, NULL, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"mainmsgqueueworkerthreads", 0, eCmdHdlrInt, NULL, &iMainMsgQueueNumWorkers, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"mainmsgqueuetimeoutshutdown", 0, eCmdHdlrInt, NULL, &iMainMsgQtoQShutdown, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"mainmsgqueuetimeoutactioncompletion", 0, eCmdHdlrInt, NULL, &iMainMsgQtoActShutdown, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"mainmsgqueuetimeoutenqueue", 0, eCmdHdlrInt, NULL, &iMainMsgQtoEnq, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"mainmsgqueueworkertimeoutthreadshutdown", 0, eCmdHdlrInt, NULL, &iMainMsgQtoWrkShutdown, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"mainmsgqueuedequeueslowdown", 0, eCmdHdlrInt, NULL, &iMainMsgQDeqSlowdown, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"mainmsgqueueworkerthreadminimummessages", 0, eCmdHdlrInt, NULL, &iMainMsgQWrkMinMsgs, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"mainmsgqueuemaxfilesize", 0, eCmdHdlrSize, NULL, &iMainMsgQueMaxFileSize, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"mainmsgqueuemaxdiskspace", 0, eCmdHdlrSize, NULL, &iMainMsgQueMaxDiskSpace, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"mainmsgqueuesaveonshutdown", 0, eCmdHdlrBinary, NULL, &bMainMsgQSaveOnShutdown, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"mainmsgqueuedequeuetimebegin", 0, eCmdHdlrInt, NULL, &iMainMsgQueueDeqtWinFromHr, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"mainmsgqueuedequeuetimeend", 0, eCmdHdlrInt, NULL, &iMainMsgQueueDeqtWinToHr, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"repeatedmsgreduction", 0, eCmdHdlrBinary, NULL, &bReduceRepeatMsgs, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"actionexeconlywhenpreviousissuspended", 0, eCmdHdlrBinary, NULL, &bActExecWhenPrevSusp, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"actionexeconlyonceeveryinterval", 0, eCmdHdlrInt, NULL, &iActExecOnceInterval, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"actionresumeinterval", 0, eCmdHdlrInt, setActionResumeInterval, NULL, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"controlcharacterescapeprefix", 0, eCmdHdlrGetChar, NULL, &cCCEscapeChar, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"escapecontrolcharactersonreceive", 0, eCmdHdlrBinary, NULL, &bEscapeCCOnRcv, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"droptrailinglfonreception", 0, eCmdHdlrBinary, NULL, &bDropTrailingLF, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"template", 0, eCmdHdlrCustomHandler, conf.doNameLine, (void*)DIR_TEMPLATE, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"outchannel", 0, eCmdHdlrCustomHandler, conf.doNameLine, (void*)DIR_OUTCHANNEL, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"allowedsender", 0, eCmdHdlrCustomHandler, conf.doNameLine, (void*)DIR_ALLOWEDSENDER, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"modload", 0, eCmdHdlrCustomHandler, conf.doModLoad, NULL, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"includeconfig", 0, eCmdHdlrCustomHandler, conf.doIncludeLine, NULL, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"umask", 0, eCmdHdlrFileCreateMode, setUmask, NULL, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"maxopenfiles", 0, eCmdHdlrInt, setMaxFiles, NULL, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"debugprinttemplatelist", 0, eCmdHdlrBinary, NULL, &bDebugPrintTemplateList, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"debugprintmodulelist", 0, eCmdHdlrBinary, NULL, &bDebugPrintModuleList, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"debugprintcfsyslinehandlerlist", 0, eCmdHdlrBinary,
		 NULL, &bDebugPrintCfSysLineHandlerList, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"moddir", 0, eCmdHdlrGetWord, NULL, &pModDir, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"generateconfiggraph", 0, eCmdHdlrGetWord, NULL, &pszConfDAGFile, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"resetconfigvariables", 1, eCmdHdlrCustomHandler, resetConfigVariables, NULL, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"errormessagestostderr", 0, eCmdHdlrBinary, NULL, &bErrMsgToStderr, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"maxmessagesize", 0, eCmdHdlrSize, setMaxMsgSize, NULL, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"privdroptouser", 0, eCmdHdlrUID, NULL, &uidDropPriv, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"privdroptouserid", 0, eCmdHdlrInt, NULL, &uidDropPriv, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"privdroptogroup", 0, eCmdHdlrGID, NULL, &gidDropPriv, NULL));
	CHKiRet(regCfSysLineHdlr((uchar *)"privdroptogroupid", 0, eCmdHdlrGID, NULL, &gidDropPriv, NULL));

	/* now add other modules handlers (we should work on that to be able to do it in ClassInit(), but so far
	 * that is not possible). -- rgerhards, 2008-01-28
	 */
	CHKiRet(actionAddCfSysLineHdrl());

finalize_it:
	RETiRet;
}