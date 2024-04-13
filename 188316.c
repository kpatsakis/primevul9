int git_path_from_url_or_path(git_buf *local_path_out, const char *url_or_path)
{
	if (git_path_is_local_file_url(url_or_path))
		return git_path_fromurl(local_path_out, url_or_path);
	else
		return git_buf_sets(local_path_out, url_or_path);
}