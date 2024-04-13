std::string directory_name(const std::string& file)
{
	return path(file).parent_path().string();
}