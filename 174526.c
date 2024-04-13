email_open_implementation( const char * final_args[])
{

	FILE *mailerstream;
	pid_t pid;
	int pipefds[2];

	/* The gist of this code is to exec a mailer whose stdin is dup2'ed onto
		the write end of a pipe. The parent gets the fdopen'ed read end
		so it looks like a FILE*. The child goes out of its
		way to set its real uid to condor and prop up the environment so
		that any mail that gets sent from the condor daemons ends up as
		comming from the condor account instead of superuser. 

		On some OS'es, the child cannot write to the logs even though the
		mailer process is ruid condor. So I turned off logging in the
		child. I have no clue why this behaviour happens.

		-pete 04/14/2000
	*/

	if (pipe(pipefds) < 0)
	{
		dprintf(D_ALWAYS, "Could not open email pipe!\n");
		return NULL;
	}

	dprintf(D_FULLDEBUG, "Forking Mailer process...\n");
	if ((pid = fork()) < 0)
	{
		dprintf(D_ALWAYS, "Could not fork email process!\n");
		return NULL;
	}
	else if (pid > 0) /* parent */
	{
		/* SIGCLD, SIGPIPE are ignored elsewhere in the code.... */

		/* close read end of pipe */
		close(pipefds[0]);

		mailerstream = fdopen(pipefds[1], EMAIL_POPEN_FLAGS);
		if (mailerstream == NULL)
		{
			dprintf(D_ALWAYS, "Could not open email FILE*: %s\n", 
				strerror(errno));
			return NULL;
		}
		return mailerstream;
	}
	else /* child mailer process */
	{
		/* Sorry, putenv wants it this way */
		char *pe_logname = (char *)malloc(256);
		char *pe_user = (char *)malloc(256);
		const char *condor_name;
		int i;

		/* Disable any EXCEPT_Cleanup code installed by the parent process.
		   Otherwise, for example, in the master, any call to EXCEPT in
		   the following code will cause us to kill the master's children. */
		_EXCEPT_Cleanup = NULL;

		/* XXX This must be the FIRST thing in this block of code. For some
			reason, at least on IRIX65, this forked process
			will not be able to open the shadow lock file,
			or be able to use dprintf or do any sort of
			logging--even if the ruid hasn't changed. I do
			not know why and this should be investigated. So
			for now, I've turned off logging for this child
			process. Thankfully it is a short piece of code
			before the exec.  -pete 03-05-2000
		*/
		dprintf_set_tool_debug("TOOL", 0);

		/* this is a simple daemon that if it needs to stat . should be
			able to. You might not be able to if the shadow's cwd is in the
			user dir somewhere and not readable by the Condor Account. */
		int ret = chdir("/");
		if (ret == -1) {
			EXCEPT("EMAIL PROCESS: Could not cd /");
		}
		umask(0);

		/* Change my userid permanently to "condor" */
		/* WARNING  This code must happen before the close/dup operation. */
		set_condor_priv_final();

		/* close write end of pipe */
		close(pipefds[1]);

		/* connect the write end of the pipe to the stdin of the mailer */
		if (dup2(pipefds[0], STDIN_FILENO) < 0)
		{
			/* I hope this EXCEPT gets recorded somewhere */
			EXCEPT("EMAIL PROCESS: Could not connect stdin to child!");
		}

		/* close all other unneeded file descriptors including stdout and
			stderr, just leave the stdin open to this process. */
		for(i = 0; i < sysconf(_SC_OPEN_MAX); i++)
		{
			if (i != pipefds[0] && i != STDIN_FILENO)
			{
				(void)close(i);
			}
		}

		/* prop up the environment with goodies to get the Mailer to do the
			right thing */
		condor_name = get_condor_username();

		/* Should be snprintf() but we don't have it for all platforms */
		sprintf(pe_logname,"LOGNAME=%s", condor_name);
		if (putenv(pe_logname) != 0)
		{
			EXCEPT("EMAIL PROCESS: Unable to insert LOGNAME=%s into "
				" environment correctly: %s\n", pe_logname, strerror(errno));
		}

		/* Should be snprintf() but we don't have it for all platforms */
		sprintf(pe_user,"USER=%s", condor_name);
		if( putenv(pe_user) != 0)
		{
			/* I hope this EXCEPT gets recorded somewhere */
			EXCEPT("EMAIL PROCESS: Unable to insert USER=%s into "
				" environment correctly: %s\n", pe_user, strerror(errno));
		}

		/* invoke the mailer */
		execvp(final_args[0], const_cast<char *const*>(final_args) );

		/* I hope this EXCEPT gets recorded somewhere */
		EXCEPT("EMAIL PROCESS: Could not exec mailer using '%s' with command "
			"'%s' because of error: %s.", "/bin/sh", 
			(final_args[0]==NULL)?"(null)":final_args[0], strerror(errno));
	}

	/* for completeness */
	return NULL;
}