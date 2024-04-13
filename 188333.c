static int path_found_entry(void *payload, git_buf *path)
{
	GIT_UNUSED(payload);
	return !git_path_is_dot_or_dotdot(path->ptr);
}