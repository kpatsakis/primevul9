static int table_parser_errcb(struct libmnt_table *tb __attribute__((__unused__)),
			const char *filename, int line)
{
	if (filename)
		warnx(_("%s: parse error: ignore entry at line %d."),
							filename, line);
	return 0;
}