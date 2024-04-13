void verify_non_filename(const char *prefix, const char *arg)
{
	if (!is_inside_work_tree() || is_inside_git_dir())
		return;
	if (*arg == '-')
		return; /* flag */
	if (!check_filename(prefix, arg))
		return;
	die("ambiguous argument '%s': both revision and filename\n"
	    "Use '--' to separate filenames from revisions", arg);
}