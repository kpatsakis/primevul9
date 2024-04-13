int git_path_prettify_dir(git_buf *path_out, const char *path, const char *base)
{
	int error = git_path_prettify(path_out, path, base);
	return (error < 0) ? error : git_path_to_dir(path_out);
}