extern int git_path_is_gitfile(const char *path, size_t pathlen, git_path_gitfile gitfile, git_path_fs fs)
{
	const char *file, *hash;
	size_t filelen;

	if (!(gitfile >= GIT_PATH_GITFILE_GITIGNORE && gitfile < ARRAY_SIZE(gitfiles))) {
		git_error_set(GIT_ERROR_OS, "invalid gitfile for path validation");
		return -1;
	}

	file = gitfiles[gitfile].file;
	filelen = gitfiles[gitfile].filelen;
	hash = gitfiles[gitfile].hash;

	switch (fs) {
	case GIT_PATH_FS_GENERIC:
		return !verify_dotgit_ntfs_generic(path, pathlen, file, filelen, hash) ||
		       !verify_dotgit_hfs_generic(path, pathlen, file, filelen);
	case GIT_PATH_FS_NTFS:
		return !verify_dotgit_ntfs_generic(path, pathlen, file, filelen, hash);
	case GIT_PATH_FS_HFS:
		return !verify_dotgit_hfs_generic(path, pathlen, file, filelen);
	default:
		git_error_set(GIT_ERROR_OS, "invalid filesystem for path validation");
		return -1;
	}
}