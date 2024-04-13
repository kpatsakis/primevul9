void git_path_trim_slashes(git_buf *path)
{
	int ceiling = git_path_root(path->ptr) + 1;
	assert(ceiling >= 0);

	while (path->size > (size_t)ceiling) {
		if (path->ptr[path->size-1] != '/')
			break;

		path->ptr[path->size-1] = '\0';
		path->size--;
	}
}