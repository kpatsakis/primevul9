static void NORETURN die_verify_filename(const char *prefix, const char *arg)
{
	unsigned char sha1[20];
	unsigned mode;
	/* try a detailed diagnostic ... */
	get_sha1_with_mode_1(arg, sha1, &mode, 0, prefix);
	/* ... or fall back the most general message. */
	die("ambiguous argument '%s': unknown revision or path not in the working tree.\n"
	    "Use '--' to separate paths from revisions", arg);

}