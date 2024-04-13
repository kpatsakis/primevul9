doGlblProcessInit(void)
{
	struct sigaction sigAct;
	int num_fds;
	int i;
	DEFiRet;

	thrdInit();

	if( !(Debug || NoFork) )
	{
		DBGPRINTF("Checking pidfile.\n");
		if (!check_pid(PidFile))
		{
			memset(&sigAct, 0, sizeof (sigAct));
			sigemptyset(&sigAct.sa_mask);
			sigAct.sa_handler = doexit;
			sigaction(SIGTERM, &sigAct, NULL);

			if (fork()) {
				/* Parent process
				 */
				sleep(300);
				/* Not reached unless something major went wrong.  5
				 * minutes should be a fair amount of time to wait.
				 * Please note that this procedure is important since
				 * the father must not exit before syslogd isn't
				 * initialized or the klogd won't be able to flush its
				 * logs.  -Joey
				 */
				exit(1); /* "good" exit - after forking, not diasabling anything */
			}
			num_fds = getdtablesize();
			close(0);
			/* we keep stdout and stderr open in case we have to emit something */
			for (i = 3; i < num_fds; i++)
				(void) close(i);
			untty();
		}
		else
		{
			fputs(" Already running.\n", stderr);
			exit(1); /* "good" exit, done if syslogd is already running */
		}
	}

	/* tuck my process id away */
	DBGPRINTF("Writing pidfile %s.\n", PidFile);
	if (!check_pid(PidFile))
	{
		if (!write_pid(PidFile))
		{
			fputs("Can't write pid.\n", stderr);
			exit(1); /* exit during startup - questionable */
		}
	}
	else
	{
		fputs("Pidfile (and pid) already exist.\n", stderr);
		exit(1); /* exit during startup - questionable */
	}
	myPid = getpid(); 	/* save our pid for further testing (also used for messages) */

	memset(&sigAct, 0, sizeof (sigAct));
	sigemptyset(&sigAct.sa_mask);

	sigAct.sa_handler = sigsegvHdlr;
	sigaction(SIGSEGV, &sigAct, NULL);
	sigAct.sa_handler = sigsegvHdlr;
	sigaction(SIGABRT, &sigAct, NULL);
	sigAct.sa_handler = doDie;
	sigaction(SIGTERM, &sigAct, NULL);
	sigAct.sa_handler = Debug ? doDie : SIG_IGN;
	sigaction(SIGINT, &sigAct, NULL);
	sigaction(SIGQUIT, &sigAct, NULL);
	sigAct.sa_handler = reapchild;
	sigaction(SIGCHLD, &sigAct, NULL);
	sigAct.sa_handler = Debug ? debug_switch : SIG_IGN;
	sigaction(SIGUSR1, &sigAct, NULL);
	sigAct.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &sigAct, NULL);
	sigaction(SIGXFSZ, &sigAct, NULL); /* do not abort if 2gig file limit is hit */

	RETiRet;
}