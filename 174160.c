std::string base_name(const std::string& file)
{
	return path(file).filename().string();
}