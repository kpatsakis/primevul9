static void doDie(int sig)
{
#	define MSG1 "DoDie called.\n"
#	define MSG2 "DoDie called 5 times - unconditional exit\n"
	static int iRetries = 0; /* debug aid */
	if(Debug)
		write(1, MSG1, sizeof(MSG1) - 1);
	if(iRetries++ == 4) {
		if(Debug)
			write(1, MSG2, sizeof(MSG2) - 1);
		abort();
	}
	bFinished = sig;
#	undef MSG1
#	undef MSG2
}