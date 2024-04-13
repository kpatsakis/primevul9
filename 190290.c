int main(int argc, char *argv[]) {
	struct sigaction sact;
	cron_db database;
	int fd;
	char *cs;
	pid_t pid = getpid();
	long oldGMToff;
#if defined WITH_INOTIFY
	int i;
#endif

	ProgramName = argv[0];
	MailCmd[0] = '\0';
	cron_default_mail_charset[0] = '\0';

	setlocale(LC_ALL, "");

#if defined(BSD)
	setlinebuf(stdout);
	setlinebuf(stderr);
#endif

	SyslogOutput = 0;
	NoFork = 0;
	parse_args(argc, argv);

	bzero((char *) &sact, sizeof sact);
	sigemptyset(&sact.sa_mask);
	sact.sa_flags = 0;
#ifdef SA_RESTART
	sact.sa_flags |= SA_RESTART;
#endif
	sact.sa_handler = sigchld_handler;
	(void) sigaction(SIGCHLD, &sact, NULL);
	sact.sa_handler = sighup_handler;
	(void) sigaction(SIGHUP, &sact, NULL);
	sact.sa_handler = quit;
	(void) sigaction(SIGINT, &sact, NULL);
	(void) sigaction(SIGTERM, &sact, NULL);

	acquire_daemonlock(0);
	set_cron_uid();
	check_spool_dir();

	if (putenv("PATH=" _PATH_DEFPATH) < 0) {
		log_it("CRON", pid, "DEATH", "can't putenv PATH", errno);
		exit(1);
	}

	/* Get the default locale character set for the mail 
	 * "Content-Type: ...; charset=" header
	 */
	setlocale(LC_ALL, "");	/* set locale to system defaults or to
							 * that specified by any  LC_* env vars */
	if ((cs = nl_langinfo(CODESET)) != 0L)
		strncpy(cron_default_mail_charset, cs, MAX_ENVSTR);
	else
		strcpy(cron_default_mail_charset, "US-ASCII");

	/* if there are no debug flags turned on, fork as a daemon should.
	 */
	if (DebugFlags) {
#if DEBUGGING
		(void) fprintf(stderr, "[%ld] cron started\n", (long) getpid());
#endif
	}
	else if (NoFork == 0) {
		switch (fork()) {
		case -1:
			log_it("CRON", pid, "DEATH", "can't fork", errno);
			exit(0);
			break;
		case 0:
			/* child process */
			(void) setsid();
			if ((fd = open(_PATH_DEVNULL, O_RDWR, 0)) >= 0) {
				(void) dup2(fd, STDIN);
				(void) dup2(fd, STDOUT);
				(void) dup2(fd, STDERR);
				if (fd != STDERR)
					(void) close(fd);
			}
			log_it("CRON", getpid(), "STARTUP", PACKAGE_VERSION, 0);
			break;
		default:
			/* parent process should just die */
			_exit(0);
		}
	}

	if (access("/usr/sbin/sendmail", X_OK) != 0) {
		SyslogOutput=1;
		log_it("CRON", pid, "INFO","Syslog will be used instead of sendmail.", errno);
	}

	pid = getpid();
	acquire_daemonlock(0);
	database.head = NULL;
	database.tail = NULL;
	database.mtime = (time_t) 0;

	load_database(&database);

	fd = -1;
#if defined WITH_INOTIFY
	if (DisableInotify || EnableClustering) {
		log_it("CRON", getpid(), "No inotify - daemon runs with -i or -c option", 
			"", 0);
	}
	else {
		for (i = 0; i < sizeof (wd) / sizeof (wd[0]); ++i) {
			/* initialize to negative number other than -1
			 * so an eventual error is reported for the first time
			 */
			wd[i] = -2;
		}

		database.ifd = fd = inotify_init();
		fcntl(fd, F_SETFD, FD_CLOEXEC);
		if (fd < 0)
			log_it("CRON", pid, "INFO", "Inotify init failed", errno);
		set_cron_watched(fd);
	}
#endif

	set_time(TRUE);
	run_reboot_jobs(&database);
	timeRunning = virtualTime = clockTime;
	oldGMToff = GMToff;

	/*
	 * Too many clocks, not enough time (Al. Einstein)
	 * These clocks are in minutes since the epoch, adjusted for timezone.
	 * virtualTime: is the time it *would* be if we woke up
	 * promptly and nobody ever changed the clock. It is
	 * monotonically increasing... unless a timejump happens.
	 * At the top of the loop, all jobs for 'virtualTime' have run.
	 * timeRunning: is the time we last awakened.
	 * clockTime: is the time when set_time was last called.
	 */
	while (TRUE) {
		int timeDiff;
		enum timejump wakeupKind;

		/* ... wait for the time (in minutes) to change ... */
		do {
			cron_sleep(timeRunning + 1, &database);
			set_time(FALSE);
		} while (clockTime == timeRunning);
		timeRunning = clockTime;

		/*
		 * Calculate how the current time differs from our virtual
		 * clock.  Classify the change into one of 4 cases.
		 */
		timeDiff = timeRunning - virtualTime;
		check_orphans(&database);
#if defined WITH_INOTIFY
		if (inotify_enabled) {
			check_inotify_database(&database);
		}
		else {
			if (load_database(&database) && (EnableClustering != 1))
				/* try reinstating the watches */
				set_cron_watched(fd);
		}
#else
		load_database(&database);
#endif

		/* shortcut for the most common case */
		if (timeDiff == 1) {
			virtualTime = timeRunning;
			oldGMToff = GMToff;
			find_jobs(virtualTime, &database, TRUE, TRUE, oldGMToff);
		}
		else {
			if (timeDiff > (3 * MINUTE_COUNT) || timeDiff < -(3 * MINUTE_COUNT))
				wakeupKind = large;
			else if (timeDiff > 5)
				wakeupKind = medium;
			else if (timeDiff > 0)
				wakeupKind = small;
			else
				wakeupKind = negative;

			switch (wakeupKind) {
			case small:
				/*
				 * case 1: timeDiff is a small positive number
				 * (wokeup late) run jobs for each virtual
				 * minute until caught up.
				 */
				Debug(DSCH, ("[%ld], normal case %d minutes to go\n",
						(long) pid, timeDiff))
				do {
					if (job_runqueue())
						sleep(10);
					virtualTime++;
					if (virtualTime >= timeRunning)
						/* always run also the other timezone jobs in the last step */
						oldGMToff = GMToff;
					find_jobs(virtualTime, &database, TRUE, TRUE, oldGMToff);
				} while (virtualTime < timeRunning);
				break;

			case medium:
				/*
				 * case 2: timeDiff is a medium-sized positive
				 * number, for example because we went to DST
				 * run wildcard jobs once, then run any
				 * fixed-time jobs that would otherwise be
				 * skipped if we use up our minute (possible,
				 * if there are a lot of jobs to run) go
				 * around the loop again so that wildcard jobs
				 * have a chance to run, and we do our
				 * housekeeping.
				 */
				Debug(DSCH, ("[%ld], DST begins %d minutes to go\n",
						(long) pid, timeDiff))
				/* run wildcard jobs for current minute */
				find_jobs(timeRunning, &database, TRUE, FALSE, GMToff);

				/* run fixed-time jobs for each minute missed */
				do {
					if (job_runqueue())
						sleep(10);
					virtualTime++;
					if (virtualTime >= timeRunning) 
						/* always run also the other timezone jobs in the last step */
						oldGMToff = GMToff;
					find_jobs(virtualTime, &database, FALSE, TRUE, oldGMToff);
					set_time(FALSE);
				} while (virtualTime < timeRunning && clockTime == timeRunning);
				break;

			case negative:
				/*
				 * case 3: timeDiff is a small or medium-sized
				 * negative num, eg. because of DST ending.
				 * Just run the wildcard jobs. The fixed-time
				 * jobs probably have already run, and should
				 * not be repeated.  Virtual time does not
				 * change until we are caught up.
				 */
				Debug(DSCH, ("[%ld], DST ends %d minutes to go\n",
						(long) pid, timeDiff))
				find_jobs(timeRunning, &database, TRUE, FALSE, GMToff);
				break;
			default:
				/*
				 * other: time has changed a *lot*,
				 * jump virtual time, and run everything
				 */
				Debug(DSCH, ("[%ld], clock jumped\n", (long) pid))
				virtualTime = timeRunning;
				oldGMToff = GMToff;
				find_jobs(timeRunning, &database, TRUE, TRUE, GMToff);
			}
		}

		/* Jobs to be run (if any) are loaded; clear the queue. */
		job_runqueue();

		handle_signals(&database);
	}

#if defined WITH_INOTIFY
	if (inotify_enabled && (EnableClustering != 1))
		set_cron_unwatched(fd);

	if (fd >= 0 && close(fd) < 0)
		log_it("CRON", pid, "INFO", "Inotify close failed", errno);
#endif
}