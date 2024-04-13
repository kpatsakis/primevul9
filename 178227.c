static int check_repository_format_gently(int *nongit_ok)
{
	git_config(check_repository_format_version, NULL);
	if (GIT_REPO_VERSION < repository_format_version) {
		if (!nongit_ok)
			die ("Expected git repo version <= %d, found %d",
			     GIT_REPO_VERSION, repository_format_version);
		warning("Expected git repo version <= %d, found %d",
			GIT_REPO_VERSION, repository_format_version);
		warning("Please upgrade Git");
		*nongit_ok = -1;
		return -1;
	}
	return 0;
}