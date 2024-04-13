parseOption(int argc, const char *argv[], int &i, Options &options) {
	OptionParser p(usage);

	if (p.isValueFlag(argc, i, argv[i], '\0', "--user")) {
		options.user = argv[i + 1];
		i += 2;
	} else {
		return false;
	}
	return true;
}