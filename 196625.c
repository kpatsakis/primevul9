usage() {
	// ....|---------------Keep output within standard terminal width (80 chars)------------|
	printf("Usage: " AGENT_EXE " exec-helper [OPTIONS...] <PROGRAM> [ARGS...]\n");
	printf("Executes the given program under a specific environment.\n");
	printf("\n");
	printf("Options:\n");
	printf("  --user <USER>   Execute as the given user. The GID will be set to the\n");
	printf("                  user's primary group. Supplementary groups will also\n");
	printf("                  be set.\n");
	printf("  --help          Show this help message.\n");
}