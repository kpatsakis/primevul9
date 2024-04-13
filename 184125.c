reapchild()
{
	int saved_errno = errno;
	struct sigaction sigAct;

	memset(&sigAct, 0, sizeof (sigAct));
	sigemptyset(&sigAct.sa_mask);
	sigAct.sa_handler = reapchild;
	sigaction(SIGCHLD, &sigAct, NULL);  /* reset signal handler -ASP */

	while(waitpid(-1, NULL, WNOHANG) > 0);
	errno = saved_errno;
}