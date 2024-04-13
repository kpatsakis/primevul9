static void logFunction(int level, const char *msg)
{
	fprintf(stdout, "DDDD: GnuTLS log msg, level %d: %s", level, msg);
	fflush(stdout);
}