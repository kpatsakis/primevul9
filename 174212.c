bool create_directory_if_missing(const std::string& dirname)
{
	return create_directory_if_missing(path(dirname));
}