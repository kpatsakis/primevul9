static int create_pin(void)
{
	sc_path_t path;
	char buf[80];

	if (opt_pin_num != 1 && opt_pin_num != 2) {
		fprintf(stderr, "Invalid PIN number. Possible values: 1, 2.\n");
		return 2;
	}
	strcpy(buf, "3F00");
	if (opt_appdf != NULL)
		strlcat(buf, opt_appdf, sizeof buf);
	sc_format_path(buf, &path);

	return create_pin_file(&path, opt_pin_num, "");
}