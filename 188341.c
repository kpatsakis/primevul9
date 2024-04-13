int git_path_apply_relative(git_buf *target, const char *relpath)
{
	return git_buf_joinpath(target, git_buf_cstr(target), relpath) ||
	    git_path_resolve_relative(target, 0);
}