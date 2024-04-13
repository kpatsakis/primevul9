daemonize(cms_context *cms_ctx, char *certdir, int do_fork)
{
	int rc = 0;
	context ctx = {
		.backup_cms = cms_ctx,
		.priority = do_fork ? LOG_PID
				    : LOG_PID|LOG_PERROR,
	};

	ctx.backup_cms = cms_ctx;
	ctx.backup_cms->log_priv = &ctx;
	ctx.sd = -1;

	if (getuid() != 0) {
		fprintf(stderr, "pesignd must be started as root");
		exit(1);
	}

	check_socket(&ctx);

	openlog("pesignd", LOG_PID, LOG_DAEMON);

	if (do_fork) {
		pid_t pid;

		if ((pid = fork())) {
			sleep(2);
			return 0;
		}
	}
	ctx.pid = getpid();
	write_pid_file(ctx.pid);
	ctx.backup_cms->log(ctx.backup_cms, ctx.priority|LOG_NOTICE,
		"pesignd starting (pid %d)", ctx.pid);
	daemon_logger(ctx.backup_cms, ctx.priority|LOG_NOTICE,
		"pesignd starting (pid %d)", ctx.pid);

	SECStatus status = NSS_Init(certdir);
	int error = errno;
	if (status != SECSuccess) {
		char *globpattern = NULL;
		rc = asprintf(&globpattern, "%s/cert*.db",
			      certdir);
		if (rc > 0) {
			glob_t globbuf;
			memset(&globbuf, 0, sizeof(globbuf));
			rc = glob(globpattern, GLOB_ERR, NULL,
				  &globbuf);
			if (rc != 0) {
				errno = error;
				ctx.backup_cms->log(ctx.backup_cms,
					ctx.priority|LOG_NOTICE,
					"Could not open NSS database (\"%s\"): %m",
					PORT_ErrorToString(PORT_GetError()));
				exit(1);
			}
		}
	}
	if (status != SECSuccess) {
		errno = error;
		ctx.backup_cms->log(ctx.backup_cms, ctx.priority|LOG_NOTICE,
				    "Could not initialize nss.\n"
				    "NSS says \"%s\" errno says \"%m\"\n",
				    PORT_ErrorToString(PORT_GetError()));
		exit(1);
	}

	status = register_oids(ctx.backup_cms);
	if (status != SECSuccess) {
		ctx.backup_cms->log(ctx.backup_cms, ctx.priority|LOG_NOTICE,
			"Could not register OIDs\n");
		exit(1);
	}

	if (do_fork) {
		int fd = open("/dev/zero", O_RDONLY);
		if (fd < 0) {
			ctx.backup_cms->log(ctx.backup_cms,
					ctx.priority|LOG_ERR,
					"could not open /dev/zero: %m");
			exit(1);
		}
		close(STDIN_FILENO);
		rc = dup2(fd, STDIN_FILENO);
		if (rc < 0) {
			ctx.backup_cms->log(ctx.backup_cms,
				ctx.priority|LOG_ERR,
				"could not set up standard input: %m");
			exit(1);
		}
		close(fd);

		fd = open("/dev/null", O_WRONLY);
		if (fd < 0) {
			ctx.backup_cms->log(ctx.backup_cms,
					ctx.priority|LOG_ERR,
					"could not open /dev/null: %m");
			exit(1);
		}
		close(STDOUT_FILENO);
		rc = dup2(fd, STDOUT_FILENO);
		if (rc < 0) {
			ctx.backup_cms->log(ctx.backup_cms,
				ctx.priority|LOG_ERR,
				"could not set up standard output: %m");
			exit(1);
		}

		close(STDERR_FILENO);
		rc = dup2(fd, STDERR_FILENO);
		if (rc < 0) {
			ctx.backup_cms->log(ctx.backup_cms,
				ctx.priority|LOG_ERR,
				"could not set up standard error: %m");
			exit(1);
		}
		close(fd);
	}

	prctl(PR_SET_NAME, "pesignd", 0, 0, 0);

	setsid();

	if (do_fork) {
		struct sigaction sa = {
			.sa_handler = quit_handler,
		};
		sigaction(SIGQUIT, &sa, NULL);
		sigaction(SIGINT, &sa, NULL);
		sigaction(SIGTERM, &sa, NULL);
	}

	char *homedir = NULL;

	rc = get_uid_and_gid(&ctx, &homedir);
	if (rc < 0) {
		ctx.backup_cms->log(ctx.backup_cms, ctx.priority|LOG_ERR,
			"could not get group and user information "
			"for pesign: %m");
		exit(1);
	}

	chdir(homedir ? homedir : "/");

	if (getuid() == 0) {
		/* process is running as root, drop privileges */
		if (setgid(ctx.gid) != 0 || setgroups(0, NULL)) {
			ctx.backup_cms->log(ctx.backup_cms,
				ctx.priority|LOG_ERR,
				"unable to drop group privileges: %m");
			exit(1);
		}
		if (setuid(ctx.uid) != 0) {
			ctx.backup_cms->log(ctx.backup_cms,
				ctx.priority|LOG_ERR,
				"unable to drop user privileges: %m");
			exit(1);
		}
	}

	set_up_socket(&ctx);

	cms_set_pw_callback(ctx.backup_cms, get_password_fail);
	cms_set_pw_data(ctx.backup_cms, NULL);
	if (do_fork)
		ctx.backup_cms->log = daemon_logger;

	rc = handle_events(&ctx);

	status = NSS_Shutdown();
	if (status != SECSuccess) {
		ctx.backup_cms->log(ctx.backup_cms, ctx.priority|LOG_ERR,
			"NSS_Shutdown failed: %s\n",
			PORT_ErrorToString(PORT_GetError()));
		exit(1);
	}
	return rc;
}