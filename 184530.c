isSignalReservedByJVM(jint sigNum)
{
	return
#if defined(SIGFPE)
		(SIGFPE == sigNum) ||
#endif /* defined(SIGFPE) */
#if defined(SIGILL)
		(SIGILL == sigNum) ||
#endif /* defined(SIGILL) */
#if defined(SIGSEGV)
		(SIGSEGV == sigNum) ||
#endif /* defined(SIGSEGV) */
#if defined(SIGBUS)
		(SIGBUS == sigNum) ||
#endif /* defined(SIGBUS) */
#if defined(SIGTRAP)
		(SIGTRAP == sigNum) ||
#endif /* defined(SIGTRAP) */
#if defined(SIGQUIT)
		(SIGQUIT == sigNum) ||
#endif /* defined(SIGQUIT) */
#if defined(SIGBREAK)
		(SIGBREAK == sigNum) ||
#endif /* defined(SIGBREAK) */
#if defined(SIGABRT)
		(SIGABRT == sigNum) ||
#endif /* defined(SIGABRT) */
#if defined(SIGCHLD)
		(SIGCHLD == sigNum) ||
#endif /* defined(SIGCHLD) */
#if defined(SIGRECONFIG)
		(SIGRECONFIG == sigNum) ||
#endif /* defined(SIGRECONFIG) */
#if defined(J9ZOS390) && defined(SIGUSR1)
		(SIGUSR1 == sigNum) ||
#endif /* defined(J9ZOS390) && defined(SIGUSR1) */
		FALSE;
}