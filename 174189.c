bool delete_directory(const std::string& dirname, const bool keep_pbl)
{
	bool ret = true;
	std::vector<std::string> files;
	std::vector<std::string> dirs;
	error_code ec;

	get_files_in_dir(dirname, &files, &dirs, ENTIRE_FILE_PATH, keep_pbl ? SKIP_PBL_FILES : NO_FILTER);

	if(!files.empty()) {
		for(std::vector<std::string>::const_iterator i = files.begin(); i != files.end(); ++i) {
			bfs::remove(path(*i), ec);
			if (ec) {
				LOG_FS << "remove(" << (*i) << "): " << ec.message() << '\n';
				ret = false;
			}
		}
	}

	if(!dirs.empty()) {
		for(std::vector<std::string>::const_iterator j = dirs.begin(); j != dirs.end(); ++j) {
			//TODO: this does not preserve any other PBL files
			// filesystem.cpp does this too, so this might be intentional
			if(!delete_directory(*j))
				ret = false;
		}
	}

	if (ret) {
		bfs::remove(path(dirname), ec);
		if (ec) {
			LOG_FS << "remove(" << dirname << "): " << ec.message() << '\n';
			ret = false;
		}
	}
	return ret;
}