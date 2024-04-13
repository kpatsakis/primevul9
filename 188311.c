int git_path_fromurl(git_buf *local_path_out, const char *file_url)
{
	int offset;

	assert(local_path_out && file_url);

	if ((offset = local_file_url_prefixlen(file_url)) < 0 ||
		file_url[offset] == '\0' || file_url[offset] == '/')
		return error_invalid_local_file_uri(file_url);

#ifndef GIT_WIN32
	offset--;	/* A *nix absolute path starts with a forward slash */
#endif

	git_buf_clear(local_path_out);
	return git__percent_decode(local_path_out, file_url + offset);
}