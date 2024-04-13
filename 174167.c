std::string get_dir(const std::string& dir_path)
{
	DIR* dir = opendir(dir_path.c_str());
	if(dir == NULL) {
		const int res = mkdir(dir_path.c_str(),AccessMode);
		if(res == 0) {
			dir = opendir(dir_path.c_str());
		} else {
			ERR_FS << "could not open or create directory: " << dir_path << std::endl;
		}
	}

	if(dir == NULL)
		return "";

	closedir(dir);

	return dir_path;
}