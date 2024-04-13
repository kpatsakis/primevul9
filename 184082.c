static int usage(void)
{
	fprintf(stderr, "usage: rsyslogd [-c<version>] [-46AdnqQvwx] [-l<hostlist>] [-s<domainlist>]\n"
			"                [-f<conffile>] [-i<pidfile>] [-N<level>] [-M<module load path>]\n"
			"                [-u<number>]\n"
			"To run rsyslogd in native mode, use \"rsyslogd -c3 <other options>\"\n\n"
			"For further information see http://www.rsyslog.com/doc\n");
	exit(1); /* "good" exit - done to terminate usage() */
}