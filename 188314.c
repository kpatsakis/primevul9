int git_path_to_dir(git_buf *path)
{
	if (path->asize > 0 &&
		git_buf_len(path) > 0 &&
		path->ptr[git_buf_len(path) - 1] != '/')
		git_buf_putc(path, '/');

	return git_buf_oom(path) ? -1 : 0;
}