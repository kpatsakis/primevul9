static int error_invalid_local_file_uri(const char *uri)
{
	git_error_set(GIT_ERROR_CONFIG, "'%s' is not a valid local file URI", uri);
	return -1;
}