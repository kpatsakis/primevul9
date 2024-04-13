void verify_filename(const char *prefix, const char *arg)
{
	if (*arg == '-')
		die("bad flag '%s' used after filename", arg);
	if (check_filename(prefix, arg))
		return;
	die_verify_filename(prefix, arg);
}