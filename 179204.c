static int check_setuid(void)
{
	if (geteuid()) {
		fprintf(stderr, "This program is not installed setuid root - "
			" \"user\" CIFS mounts not supported.\n");
		return EX_USAGE;
	}

#if CIFS_DISABLE_SETUID_CAPABILITY
	if (getuid() && !geteuid()) {
		printf("This program has been built with the "
		       "ability to run as a setuid root program disabled.\n");
		return EX_USAGE;
	}
#endif /* CIFS_DISABLE_SETUID_CAPABILITY */

	return 0;
}