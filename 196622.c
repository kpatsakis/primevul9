describeCommand(int argc, const char *argv[], const Options &options) {
	string result = "'";
	result.append(argv[options.programArgStart]);
	result.append("'");

	if (argc > options.programArgStart + 1) {
		result.append(" (with params '");

		int i = options.programArgStart + 1;
		while (i < argc) {
			if (i != options.programArgStart + 1) {
				result.append(" ");
			}
			result.append(argv[i]);
			i++;
		}

		result.append("')");
	}

	return result;
}