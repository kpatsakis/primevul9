std::string get_binary_file_location(const std::string& type, const std::string& filename)
{
	DBG_FS << "Looking for '" << filename << "'." << std::endl;

	if (filename.empty()) {
		LOG_FS << "  invalid filename (type: " << type <<")" << std::endl;
		return std::string();
	}

	// Some parts of Wesnoth enjoy putting ".." inside filenames. This is
	// bad and should be fixed. But in the meantime, deal with them in a dumb way.
	std::string::size_type pos = filename.rfind("../");
	if (pos != std::string::npos) {
		std::string nf = filename.substr(pos + 3);
		LOG_FS << "Illegal path '" << filename << "' replaced by '" << nf << "'" << std::endl;
		return get_binary_file_location(type, nf);
	}

	if (filename.find("..") != std::string::npos) {
		ERR_FS << "Illegal path '" << filename << "' (\"..\" not allowed)." << std::endl;
		return std::string();
	}

	BOOST_FOREACH(const std::string &path, get_binary_paths(type))
	{
		const std::string file = path + filename;
		DBG_FS << "  checking '" << path << "'" << std::endl;
		if(file_exists(file)) {
			DBG_FS << "  found at '" << file << "'" << std::endl;
			return file;
		}
	}

	DBG_FS << "  not found" << std::endl;
	return std::string();
}