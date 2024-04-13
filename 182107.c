static void check_no_capabilities(const char *line, int len)
{
	if (strlen(line) != len)
		warning(_("ignoring capabilities after first line '%s'"),
			line + strlen(line));
}