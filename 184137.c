static void printVersion(void)
{
	printf("rsyslogd %s, ", VERSION);
	printf("compiled with:\n");
#ifdef FEATURE_REGEXP
	printf("\tFEATURE_REGEXP:\t\t\t\tYes\n");
#else
	printf("\tFEATURE_REGEXP:\t\t\t\tNo\n");
#endif
#if defined(_LARGE_FILES) || (defined (_FILE_OFFSET_BITS) && _FILE_OFFSET_BITS >= 64)
	printf("\tFEATURE_LARGEFILE:\t\t\tYes\n");
#else
	printf("\tFEATURE_LARGEFILE:\t\t\tNo\n");
#endif
#ifdef	USE_NETZIP
	printf("\tFEATURE_NETZIP (message compression):\tYes\n");
#else
	printf("\tFEATURE_NETZIP (message compression):\tNo\n");
#endif
#if defined(SYSLOG_INET) && defined(USE_GSSAPI)
	printf("\tGSSAPI Kerberos 5 support:\t\tYes\n");
#else
	printf("\tGSSAPI Kerberos 5 support:\t\tNo\n");
#endif
#ifndef	NDEBUG
	printf("\tFEATURE_DEBUG (debug build, slow code):\tYes\n");
#else
	printf("\tFEATURE_DEBUG (debug build, slow code):\tNo\n");
#endif
#ifdef	HAVE_ATOMIC_BUILTINS
	printf("\tAtomic operations supported:\t\tYes\n");
#else
	printf("\tAtomic operations supported:\t\tNo\n");
#endif
#ifdef	RTINST
	printf("\tRuntime Instrumentation (slow code):\tYes\n");
#else
	printf("\tRuntime Instrumentation (slow code):\tNo\n");
#endif
	printf("\nSee http://www.rsyslog.com for more information.\n");
}