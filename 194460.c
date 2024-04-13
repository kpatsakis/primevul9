void test_path_dotgit__dotgit_modules(void)
{
	size_t i;

	cl_assert_equal_i(1, git_path_is_gitfile(".gitmodules", strlen(".gitmodules"), GIT_PATH_GITFILE_GITMODULES, GIT_PATH_FS_GENERIC));
	cl_assert_equal_i(1, git_path_is_gitfile(".git\xe2\x80\x8cmodules", strlen(".git\xe2\x80\x8cmodules"), GIT_PATH_GITFILE_GITMODULES, GIT_PATH_FS_GENERIC));

	for (i = 0; i < ARRAY_SIZE(gitmodules_altnames); i++) {
		const char *name = gitmodules_altnames[i];
		if (!git_path_is_gitfile(name, strlen(name), GIT_PATH_GITFILE_GITMODULES, GIT_PATH_FS_GENERIC))
			cl_fail(name);
	}

	for (i = 0; i < ARRAY_SIZE(gitmodules_not_altnames); i++) {
		const char *name = gitmodules_not_altnames[i];
		if (git_path_is_gitfile(name, strlen(name), GIT_PATH_GITFILE_GITMODULES, GIT_PATH_FS_GENERIC))
			cl_fail(name);
	}
}