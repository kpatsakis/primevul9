init()
{
	rsRetVal localRet;
	int iNbrActions;
	int bHadConfigErr = 0;
	ruleset_t *pRuleset;
	char cbuf[BUFSIZ];
	char bufStartUpMsg[512];
	struct sigaction sigAct;
	DEFiRet;

	thrdTerminateAll(); /* stop all running input threads - TODO: reconsider location! */

	/* initialize some static variables */
	pDfltHostnameCmp = NULL;
	pDfltProgNameCmp = NULL;
	eDfltHostnameCmpMode = HN_NO_COMP;

	DBGPRINTF("rsyslog %s - called init()\n", VERSION);

	/* delete the message queue, which also flushes all messages left over */
	if(pMsgQueue != NULL) {
		DBGPRINTF("deleting main message queue\n");
		qqueueDestruct(&pMsgQueue); /* delete pThis here! */
		pMsgQueue = NULL;
	}

	/*  Close all open log files and free log descriptor array. This also frees
	 *  all output-modules instance data.
	 */
	destructAllActions();

	/* Unload all non-static modules */
	DBGPRINTF("Unloading non-static modules.\n");
	module.UnloadAndDestructAll(eMOD_LINK_DYNAMIC_LOADED);

	DBGPRINTF("Clearing templates.\n");
	tplDeleteNew();

	/* re-setting values to defaults (where applicable) */
	/* once we have loadable modules, we must re-visit this code. The reason is
	 * that config variables are not re-set, because the module is not yet loaded. On
	 * the other hand, that doesn't matter, because the module got unloaded and is then
	 * re-loaded, so the variables should be re-set via that way. And this is exactly how
	 * it works. Loadable module's variables are initialized on load, the rest here.
	 * rgerhards, 2008-04-28
	 */
	conf.cfsysline((uchar*)"ResetConfigVariables");

	conf.ReInitConf();

	/* construct the default ruleset */
	ruleset.Construct(&pRuleset);
	ruleset.SetName(pRuleset, UCHAR_CONSTANT("RSYSLOG_DefaultRuleset"));
	ruleset.ConstructFinalize(pRuleset);

	/* open the configuration file */
	localRet = conf.processConfFile(ConfFile);
	CHKiRet(conf.GetNbrActActions(&iNbrActions));

	if(localRet != RS_RET_OK) {
		errmsg.LogError(0, localRet, "CONFIG ERROR: could not interpret master config file '%s'.", ConfFile);
		bHadConfigErr = 1;
	} else if(iNbrActions == 0) {
		errmsg.LogError(0, RS_RET_NO_ACTIONS, "CONFIG ERROR: there are no active actions configured. Inputs will "
			 "run, but no output whatsoever is created.");
		bHadConfigErr = 1;
	}

	if((localRet != RS_RET_OK && localRet != RS_RET_NONFATAL_CONFIG_ERR) || iNbrActions == 0) {
		/* rgerhards: this code is executed to set defaults when the
		 * config file could not be opened. We might think about
		 * abandoning the run in this case - but this, too, is not
		 * very clever... So we stick with what we have.
		 * We ignore any errors while doing this - we would be lost anyhow...
		 */
		errmsg.LogError(0, NO_ERRCODE, "EMERGENCY CONFIGURATION ACTIVATED - fix rsyslog config file!");

		/* note: we previously used _POSIY_TTY_NAME_MAX+1, but this turned out to be
		 * too low on linux... :-S   -- rgerhards, 2008-07-28
		 */
		char szTTYNameBuf[128];
		rule_t *pRule = NULL; /* initialization to NULL is *vitally* important! */
		conf.cfline(UCHAR_CONSTANT("*.ERR\t" _PATH_CONSOLE), &pRule);
		conf.cfline(UCHAR_CONSTANT("syslog.*\t" _PATH_CONSOLE), &pRule);
		conf.cfline(UCHAR_CONSTANT("*.PANIC\t*"), &pRule);
		conf.cfline(UCHAR_CONSTANT("syslog.*\troot"), &pRule);
		if(ttyname_r(0, szTTYNameBuf, sizeof(szTTYNameBuf)) == 0) {
			snprintf(cbuf,sizeof(cbuf), "*.*\t%s", szTTYNameBuf);
			conf.cfline((uchar*)cbuf, &pRule);
		} else {
			DBGPRINTF("error %d obtaining controlling terminal, not using that emergency rule\n", errno);
		}
		ruleset.AddRule(ruleset.GetCurrent(), &pRule);
	}

	legacyOptsHook();

	/* we are now done with reading the configuration. This is the right time to
	 * free some objects that were just needed for loading it. rgerhards 2005-10-19
	 */
	if(pDfltHostnameCmp != NULL) {
		rsCStrDestruct(&pDfltHostnameCmp);
	}

	if(pDfltProgNameCmp != NULL) {
		rsCStrDestruct(&pDfltProgNameCmp);
	}

	/* some checks */
	if(iMainMsgQueueNumWorkers < 1) {
		errmsg.LogError(0, NO_ERRCODE, "$MainMsgQueueNumWorkers must be at least 1! Set to 1.\n");
		iMainMsgQueueNumWorkers = 1;
	}

	if(MainMsgQueType == QUEUETYPE_DISK) {
		errno = 0;	/* for logerror! */
		if(glbl.GetWorkDir() == NULL) {
			errmsg.LogError(0, NO_ERRCODE, "No $WorkDirectory specified - can not run main message queue in 'disk' mode. "
				 "Using 'FixedArray' instead.\n");
			MainMsgQueType = QUEUETYPE_FIXED_ARRAY;
		}
		if(pszMainMsgQFName == NULL) {
			errmsg.LogError(0, NO_ERRCODE, "No $MainMsgQueueFileName specified - can not run main message queue in "
				 "'disk' mode. Using 'FixedArray' instead.\n");
			MainMsgQueType = QUEUETYPE_FIXED_ARRAY;
		}
	}

	/* check if we need to generate a config DAG and, if so, do that */
	if(pszConfDAGFile != NULL)
		generateConfigDAG(pszConfDAGFile);

	/* we are done checking the config - now validate if we should actually run or not.
	 * If not, terminate. -- rgerhards, 2008-07-25
	 */
	if(iConfigVerify) {
		if(bHadConfigErr) {
			/* a bit dirty, but useful... */
			exit(1);
		}
		ABORT_FINALIZE(RS_RET_VALIDATION_RUN);
	}

	/* switch the message object to threaded operation, if necessary */
	if(MainMsgQueType == QUEUETYPE_DIRECT || iMainMsgQueueNumWorkers > 1) {
		MsgEnableThreadSafety();
	}

	/* create message queue */
	CHKiRet_Hdlr(qqueueConstruct(&pMsgQueue, MainMsgQueType, iMainMsgQueueNumWorkers, iMainMsgQueueSize, msgConsumer)) {
		/* no queue is fatal, we need to give up in that case... */
		fprintf(stderr, "fatal error %d: could not create message queue - rsyslogd can not run!\n", iRet);
		exit(1);
	}
	/* name our main queue object (it's not fatal if it fails...) */
	obj.SetName((obj_t*) pMsgQueue, (uchar*) "main Q");

	/* ... set some properties ... */
#	define setQPROP(func, directive, data) \
	CHKiRet_Hdlr(func(pMsgQueue, data)) { \
		errmsg.LogError(0, NO_ERRCODE, "Invalid " #directive ", error %d. Ignored, running with default setting", iRet); \
	}
#	define setQPROPstr(func, directive, data) \
	CHKiRet_Hdlr(func(pMsgQueue, data, (data == NULL)? 0 : strlen((char*) data))) { \
		errmsg.LogError(0, NO_ERRCODE, "Invalid " #directive ", error %d. Ignored, running with default setting", iRet); \
	}

	setQPROP(qqueueSetMaxFileSize, "$MainMsgQueueFileSize", iMainMsgQueMaxFileSize);
	setQPROP(qqueueSetsizeOnDiskMax, "$MainMsgQueueMaxDiskSpace", iMainMsgQueMaxDiskSpace);
	setQPROPstr(qqueueSetFilePrefix, "$MainMsgQueueFileName", pszMainMsgQFName);
	setQPROP(qqueueSetiPersistUpdCnt, "$MainMsgQueueCheckpointInterval", iMainMsgQPersistUpdCnt);
	setQPROP(qqueueSetbSyncQueueFiles, "$MainMsgQueueSyncQueueFiles", bMainMsgQSyncQeueFiles);
	setQPROP(qqueueSettoQShutdown, "$MainMsgQueueTimeoutShutdown", iMainMsgQtoQShutdown );
	setQPROP(qqueueSettoActShutdown, "$MainMsgQueueTimeoutActionCompletion", iMainMsgQtoActShutdown);
	setQPROP(qqueueSettoWrkShutdown, "$MainMsgQueueWorkerTimeoutThreadShutdown", iMainMsgQtoWrkShutdown);
	setQPROP(qqueueSettoEnq, "$MainMsgQueueTimeoutEnqueue", iMainMsgQtoEnq);
	setQPROP(qqueueSetiHighWtrMrk, "$MainMsgQueueHighWaterMark", iMainMsgQHighWtrMark);
	setQPROP(qqueueSetiLowWtrMrk, "$MainMsgQueueLowWaterMark", iMainMsgQLowWtrMark);
	setQPROP(qqueueSetiDiscardMrk, "$MainMsgQueueDiscardMark", iMainMsgQDiscardMark);
	setQPROP(qqueueSetiDiscardSeverity, "$MainMsgQueueDiscardSeverity", iMainMsgQDiscardSeverity);
	setQPROP(qqueueSetiMinMsgsPerWrkr, "$MainMsgQueueWorkerThreadMinimumMessages", iMainMsgQWrkMinMsgs);
	setQPROP(qqueueSetbSaveOnShutdown, "$MainMsgQueueSaveOnShutdown", bMainMsgQSaveOnShutdown);
	setQPROP(qqueueSetiDeqSlowdown, "$MainMsgQueueDequeueSlowdown", iMainMsgQDeqSlowdown);
	setQPROP(qqueueSetiDeqtWinFromHr,  "$MainMsgQueueDequeueTimeBegin", iMainMsgQueueDeqtWinFromHr);
	setQPROP(qqueueSetiDeqtWinToHr,    "$MainMsgQueueDequeueTimeEnd", iMainMsgQueueDeqtWinToHr);

#	undef setQPROP
#	undef setQPROPstr

	/* ... and finally start the queue! */
	CHKiRet_Hdlr(qqueueStart(pMsgQueue)) {
		/* no queue is fatal, we need to give up in that case... */
		fprintf(stderr, "fatal error %d: could not start message queue - rsyslogd can not run!\n", iRet);
		exit(1);
	}

	bHaveMainQueue = (MainMsgQueType == QUEUETYPE_DIRECT) ? 0 : 1;
	DBGPRINTF("Main processing queue is initialized and running\n");

	/* the output part and the queue is now ready to run. So it is a good time
	 * to initialize the inputs. Please note that the net code above should be
	 * shuffled to down here once we have everything in input modules.
	 * rgerhards, 2007-12-14
	 * NOTE: as of 2009-06-29, the input modules are initialized, but not yet run.
	 * Keep in mind. though, that the outputs already run if the queue was
	 * persisted to disk. -- rgerhards
	 */
	startInputModules();

	if(Debug) {
		dbgPrintInitInfo();
	}

	/* we now generate the startup message. It now includes everything to
	 * identify this instance. -- rgerhards, 2005-08-17
	 */
	snprintf(bufStartUpMsg, sizeof(bufStartUpMsg)/sizeof(char), 
		 " [origin software=\"rsyslogd\" " "swVersion=\"" VERSION \
		 "\" x-pid=\"%d\" x-info=\"http://www.rsyslog.com\"] (re)start",
		 (int) myPid);
	logmsgInternal(NO_ERRCODE, LOG_SYSLOG|LOG_INFO, (uchar*)bufStartUpMsg, 0);

	memset(&sigAct, 0, sizeof (sigAct));
	sigemptyset(&sigAct.sa_mask);
	sigAct.sa_handler = sighup_handler;
	sigaction(SIGHUP, &sigAct, NULL);

	DBGPRINTF(" (re)started.\n");

finalize_it:
	RETiRet;
}