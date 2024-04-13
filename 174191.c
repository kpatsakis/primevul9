bool is_bzip2_file(const std::string& filename)
{
	return path(filename).extension() == ".bz2";
}