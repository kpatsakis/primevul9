bool is_path_sep(char c)
{
#ifdef _WIN32
	if (c == '/' || c == '\\') return true;
#else
	if (c == '/') return true;
#endif
	return false;
}