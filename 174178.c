int dir_size(const std::string& path)
{
	std::vector<std::string> files, dirs;
	get_files_in_dir(path, &files, &dirs, ENTIRE_FILE_PATH);

	int res = 0;

	BOOST_FOREACH(const std::string& file_path, files)
	{
		res += file_size(file_path);
	}

	BOOST_FOREACH(const std::string& dir_path, dirs)
	{
		// FIXME: this could result in infinite recursion with symlinks!!
		res += dir_size(dir_path);
	}

	return res;
}