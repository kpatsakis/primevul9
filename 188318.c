bool git_path_is_local_file_url(const char *file_url)
{
	return (local_file_url_prefixlen(file_url) > 0);
}