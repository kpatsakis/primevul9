int git_path_set_error(int errno_value, const char *path, const char *action)
{
	switch (errno_value) {
	case ENOENT:
	case ENOTDIR:
		git_error_set(GIT_ERROR_OS, "could not find '%s' to %s", path, action);
		return GIT_ENOTFOUND;

	case EINVAL:
	case ENAMETOOLONG:
		git_error_set(GIT_ERROR_OS, "invalid path for filesystem '%s'", path);
		return GIT_EINVALIDSPEC;

	case EEXIST:
		git_error_set(GIT_ERROR_OS, "failed %s - '%s' already exists", action, path);
		return GIT_EEXISTS;

	case EACCES:
		git_error_set(GIT_ERROR_OS, "failed %s - '%s' is locked", action, path);
		return GIT_ELOCKED;

	default:
		git_error_set(GIT_ERROR_OS, "could not %s '%s'", action, path);
		return -1;
	}
}