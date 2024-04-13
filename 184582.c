isSignalUsedForShutdown(jint sigNum)
{
	return
#if defined(SIGHUP)
		(SIGHUP == sigNum) ||
#endif /* defined(SIGHUP) */
#if defined(SIGINT)
		(SIGINT == sigNum) ||
#endif /* defined(SIGINT) */
#if defined(SIGTERM)
		(SIGTERM == sigNum) ||
#endif /* defined(SIGTERM) */
		FALSE;
}