bool is_gzip_file(const std::string& filename)
{
	return path(filename).extension() == ".gz";
}