bool is_directory(const std::string& fname)
{
	return is_directory_internal(path(fname));
}