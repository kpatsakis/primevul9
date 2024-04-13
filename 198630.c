util_print_usage_and_die(const char *app_name, const struct option options[],
	const char *option_help[], const char *args)
{
	int i;
	int header_shown = 0;

	if (args)
		printf("Usage: %s [OPTIONS] %s\n", app_name, args);
	else
		printf("Usage: %s [OPTIONS]\n", app_name);

	for (i = 0; options[i].name; i++) {
		char buf[40];
		const char *arg_str;

		/* Skip "hidden" options */
		if (option_help[i] == NULL)
			continue;

		if (!header_shown++)
			printf("Options:\n");

		switch (options[i].has_arg) {
		case 1:
			arg_str = " <arg>";
			break;
		case 2:
			arg_str = " [arg]";
			break;
		default:
			arg_str = "";
			break;
		}
		if (isascii(options[i].val) &&
		    isprint(options[i].val) && !isspace(options[i].val))
			sprintf(buf, "-%c, --%s%s", options[i].val, options[i].name, arg_str);
		else
			sprintf(buf, "    --%s%s", options[i].name, arg_str);

		/* print the line - wrap if necessary */
		if (strlen(buf) > 28) {
			printf("  %s\n", buf);
			buf[0] = '\0';
		}
		printf("  %-28s  %s\n", buf, option_help[i]);
	}

	exit(2);
}