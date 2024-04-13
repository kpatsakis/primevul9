static int select_app_df(void)
{
	sc_path_t path;
	sc_file_t *file;
	char str[80];
	int r;

	strcpy(str, "3F00");
	if (opt_appdf != NULL)
		strlcat(str, opt_appdf, sizeof str);
	sc_format_path(str, &path);
	r = sc_select_file(card, &path, &file);
	if (r) {
		fprintf(stderr, "Unable to select application DF: %s\n", sc_strerror(r));
		return -1;
	}
	if (file->type != SC_FILE_TYPE_DF) {
		fprintf(stderr, "Selected application DF is not a DF.\n");
		return -1;
	}
	sc_file_free(file);
	if (opt_pin_num >= 0)
		return verify_pin(opt_pin_num);
	else
		return 0;
}