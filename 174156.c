bool is_bzip2_file(const std::string& filename)
{
	return (filename.length() > 4
		&& filename.substr(filename.length() - 4) == ".bz2");
}