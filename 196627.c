execHelperMain(int argc, char *argv[]) {
	if (argc < 3) {
		usage();
		exit(1);
	}

	Options options;
	if (!parseOptions(argc, (const char **) argv, options)) {
		fprintf(stderr, "Error parsing arguments.\n");
		usage();
		exit(1);
	}

	resetSignalHandlersAndMask();
	disableMallocDebugging();

	if (!options.user.empty()) {
		struct passwd *userInfo;
		uid_t uid;
		gid_t gid;

		lookupUserGroup(options.user, &uid, &userInfo, &gid);
		switchGroup(uid, userInfo, gid);
		switchUser(uid, userInfo);
	}

	execvp(argv[options.programArgStart],
		(char * const *) &argv[options.programArgStart]);
	int e = errno;
	fprintf(stderr, "ERROR: unable to execute %s: %s (errno=%d)\n",
		describeCommand(argc, (const char **) argv, options).c_str(),
		strerror(e),
		e);
	return 1;
}