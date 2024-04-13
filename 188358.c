static bool verify_dotgit_hfs(const char *path, size_t len)
{
	return verify_dotgit_hfs_generic(path, len, "git", CONST_STRLEN("git"));
}