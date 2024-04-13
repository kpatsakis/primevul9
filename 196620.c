parseOptions(int argc, const char *argv[], Options &options) {
	OptionParser p(usage);
	int i = 2;

	while (i < argc) {
		if (parseOption(argc, argv, i, options)) {
			continue;
		} else if (p.isFlag(argv[i], 'h', "--help")) {
			usage();
			exit(0);
		} else if (*argv[i] == '-') {
			fprintf(stderr, "ERROR: unrecognized argument %s. Please type "
				"'%s exec-helper --help' for usage.\n", argv[i], argv[0]);
			exit(1);
		} else {
			options.programArgStart = i;
			return true;
		}
	}

	return true;
}