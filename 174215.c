std::string get_binary_dir_location(const std::string &type, const std::string &filename)
{
	DBG_FS << "Looking for '" << filename << "'." << std::endl;

	if (filename.empty()) {
		LOG_FS << "  invalid filename (type: " << type <<")" << std::endl;
		return std::string();
	}

	if (filename.find("..") != std::string::npos) {
		ERR_FS << "Illegal path '" << filename << "' (\"..\" not allowed)." << std::endl;
		return std::string();
	}

	BOOST_FOREACH(const std::string &path, get_binary_paths(type))
	{
		const std::string file = path + filename;
		DBG_FS << "  checking '" << path << "'" << std::endl;
		if (is_directory(file)) {
			DBG_FS << "  found at '" << file << "'" << std::endl;
			return file;
		}
	}

	DBG_FS << "  not found" << std::endl;
	return std::string();
}