bool delete_directory(const std::string& path, const bool keep_pbl)
{
	bool ret = true;
	std::vector<std::string> files;
	std::vector<std::string> dirs;

	get_files_in_dir(path, &files, &dirs, ENTIRE_FILE_PATH, keep_pbl ? SKIP_PBL_FILES : NO_FILTER);

	if(!files.empty()) {
		for(std::vector<std::string>::const_iterator i = files.begin(); i != files.end(); ++i) {
			errno = 0;
			if(remove((*i).c_str()) != 0) {
				LOG_FS << "remove(" << (*i) << "): " << strerror(errno) << std::endl;
				ret = false;
			}
		}
	}

	if(!dirs.empty()) {
		for(std::vector<std::string>::const_iterator j = dirs.begin(); j != dirs.end(); ++j) {
			if(!delete_directory(*j))
				ret = false;
		}
	}

	errno = 0;
#ifdef _WIN32
	// remove() doesn't delete directories on windows.
	int (*remove)(const char*);
	if(is_directory(path))
		remove = rmdir;
	else
		remove = ::remove;
#endif
	if(remove(path.c_str()) != 0) {
		LOG_FS << "remove(" << path << "): " << strerror(errno) << std::endl;
		ret = false;
	}
	return ret;
}