bool create_directory_if_missing_recursive(const std::string& dirname)
{
	return create_directory_if_missing_recursive(path(dirname));
}