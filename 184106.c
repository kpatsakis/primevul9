void sighup_handler()
{
	struct sigaction sigAct;
	
	bHadHUP = 1;

	memset(&sigAct, 0, sizeof (sigAct));
	sigemptyset(&sigAct.sa_mask);
	sigAct.sa_handler = sighup_handler;
	sigaction(SIGHUP, &sigAct, NULL);
}